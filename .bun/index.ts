import { watch } from "chokidar";
import * as prompts from "@clack/prompts";
import { parseArgs } from "util";

const args = parseArgs({
  options: {
    watch: { type: "boolean", default: false, multiple: false, short: "w" },
    help: { type: "boolean", default: false, multiple: false, short: "h" },
    project: { type: "string", multiple: false, short: "p" },
  },
});

if (args.values.help) {
  prompts.log.info("Help information");
  process.exit(0);
}

prompts.intro("things-in-c | Project Runner");

// Get a list of all project directories
const options: string[] = [];

for await (let item of new Bun.Glob("./*/").scan({ onlyFiles: false })) {
  item = item.replace(/^\.\//, "");
  if (!/^\d+_/g.test(item)) continue;

  options.push(item);
}

options.sort();

// Load configuration
let config: any = {};
const configFile = Bun.file("./.bun/.cfg.json");
if (await configFile.exists()) config = await configFile.json();

if (args.values.project) {
  config.selectedProject = args.values.project;
}

const selectedProject = await (async () => {
  if (args.values.project && options.includes(args.values.project)) {
    return args.values.project;
  }

  return await prompts.select({
    message: "Select a project to run",
    options: options.map((item) => ({ value: item, label: item })),
    initialValue: config.selectedProject,
  });
})();

if (prompts.isCancel(selectedProject)) process.exit(1);

// Store config back
config.selectedProject = selectedProject;
await configFile.write(JSON.stringify(config));

prompts.outro(`🏃 Running project: ${selectedProject}`);

// -------------------------------------------------------------------------------------

const dir = `./${selectedProject}/`;

let nobProcess: Bun.Subprocess | undefined;

const restartNobProcess = () => {
  console.clear();
  prompts.intro(`🏃 Running project: ${selectedProject}`);

  nobProcess?.kill(9);

  nobProcess = Bun.spawn(["./nob", selectedProject], {
    stdin: "inherit",
    stdout: "inherit",
    stderr: "inherit",
  });

  if (args.values.watch) {
    nobProcess.exited.then(() => {
      prompts.outro(`Waiting for changes inside "${dir}"...`);
    });
  }
};

if (args.values.watch) {
  const watcher = watch(dir);

  watcher.on("all", (event) => {
    if (
      event === "change" ||
      event === "add" ||
      event === "addDir" ||
      event === "unlink"
    ) {
      restartNobProcess();
    }
  });
} else {
  restartNobProcess();
}
