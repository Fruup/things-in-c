import { FSWatcher, watch } from "chokidar";
import * as prompts from "@clack/prompts";
import { parseArgs } from "util";

const args = parseArgs({
  allowPositionals: true,
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

const [project] = args.positionals;

prompts.intro("things-in-c | Project Runner");

// Get a list of all project directories
const options: string[] = [];

for await (let item of new Bun.Glob("./*/").scan({ onlyFiles: false })) {
  item = item.replace(/^\.\//, "");
  if (!/^\d/g.test(item)) continue;

  if (project && !item.includes(project)) continue;

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

const selectedProject =
  options.length === 1
    ? options[0]
    : await (async () => {
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

const restartNobProcess = async ({ watcher }: { watcher?: FSWatcher } = {}) => {
  nobProcess?.kill(9);
  console.clear();

  const dependencies = await detectDependencies(selectedProject);

  if (dependencies.length > 0) {
    prompts.log.info(`Detected dependencies: ${dependencies.join(", ")}`);

    // Also watch for changes in the dependencies
    watcher?.add(dependencies.map((dep) => `./${dep}/`));
  }

  const cmd = ["./nob", selectedProject, ...dependencies];

  prompts.note(cmd.join(" "), `🏃 Running project: ${selectedProject}`);

  nobProcess = Bun.spawn(cmd, {
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
  const watcher = watch(dir, {
    ignoreInitial: true,
  });

  watcher.on("ready", () => {
    watcher.on("all", (event, path) => {
      if (
        event === "change" ||
        event === "add" ||
        event === "addDir" ||
        event === "unlink"
      ) {
        restartNobProcess({ watcher });
      }
    });

    restartNobProcess({ watcher });
  });
} else {
  restartNobProcess();
}

/**
 * Automatically detect project dependencies by analyzing #include statements.
 */
async function detectDependencies(project: string) {
  const otherOptions = options.filter((option) => option !== project);

  const dependencies = new Set<string>();

  for await (const filename of new Bun.Glob(`./${project}/**/*.{c,h}`).scan({
    onlyFiles: true,
    absolute: true,
  })) {
    const text = await Bun.file(filename).text();

    const includeRegex = new RegExp(
      `^\\s*#include.+\\W(${otherOptions.join("|")})\\W`,
      "gim"
    );

    for (const [_, dependency] of text.matchAll(includeRegex)) {
      if (dependency) dependencies.add(dependency);
    }
  }

  return Array.from(dependencies);
}
