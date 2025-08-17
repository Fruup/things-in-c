import { watch } from "chokidar";
import * as prompts from "@clack/prompts";
import { parseArgs } from "util";

const args = parseArgs({
  options: {
    watch: { type: "boolean", default: false, multiple: false, short: "w" },
  },
});

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

const selectedProject = await prompts.select({
  message: "Select a project to run",
  options: options.map((item) => ({ value: item, label: item })),
  initialValue: config.selectedProject,
});

await configFile.write(JSON.stringify({ selectedProject }));

if (prompts.isCancel(selectedProject)) process.exit(1);

prompts.outro(`🏃 Running project: ${selectedProject}`);

// -------------------------------------------------------------------------------------

let nobProcess: Bun.Subprocess | undefined;

const restartNobProcess = () => {
  nobProcess?.kill();

  nobProcess = Bun.spawn(["./nob", selectedProject], {
    stdin: "inherit",
    stdout: "inherit",
    stderr: "inherit",
  });
};

restartNobProcess();

if (args.values.watch) {
  const dir = `./${selectedProject}/`;
  prompts.log.info(`Watching for changes inside "${dir}"...`);

  const watcher = watch(dir);

  watcher.on("change", () => {
    restartNobProcess();
  });
  watcher.on("add", () => {
    restartNobProcess();
  });
  watcher.on("addDir", () => {
    restartNobProcess();
  });
}
