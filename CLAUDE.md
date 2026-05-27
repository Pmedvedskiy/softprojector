# softprojector (personal fork)

## Context

This is a long-term personal fork of [SoftProjector/softprojector](https://github.com/SoftProjector/softprojector), maintained by pmedvedskiy. Not a contribution branch. Changes here are not expected to be merged back upstream.

Upstream is a Qt6 C++ church-presentation desktop app (lyrics, scripture, slides). Recent upstream activity has been a Qt5→Qt6 migration.

## Remotes

- `origin` → `github.com/pmedvedskiy/softprojector` (the fork)
- `upstream` → `github.com/SoftProjector/softprojector` (the source project)

Default branch is `master` (not `main`).

## Branch strategy

Long-term-fork pattern:

- `master` tracks `upstream/master`. Never commit local changes to `master`.
- Personal changes live on `pmedvedskiy/customizations` (or feature branches off it).
- To sync upstream:
  ```
  git fetch upstream
  git checkout master
  git merge --ff-only upstream/master
  git push origin master
  git checkout pmedvedskiy/customizations
  git merge master      # or rebase, depending on preference
  ```

## Project layout

- `src/softProjector.pro` is the qmake project file (this is the build entry point).
- `src/headers/`, `src/sources/` for C++.
- `src/qml/`, `src/ui/` for the UI.
- `src/translations/` for i18n.
- `3rdparty/` contains vendored dependencies.
- `help/`, `iconSource/`, `_config.yml` are docs/assets.

## Build

Build is not set up in this WSL2 environment yet. Deferred until there is something to build.

When the time comes, the typical Linux flow is:
```
cd src && qmake softProjector.pro && make
```
Requires Qt6 SDK + a C++ compiler. The upstream README only documents the Windows `windeployqt` packaging step.

## Working style for this project

- Treat any change as a fork-only change. Do not propose upstream-style PRs unless explicitly asked.
- Before large changes, check whether upstream has moved (so we are not modifying code that will conflict on next sync).
- Translations: the project has multiple language files in `src/translations/`. If adding strings, surface the `tr()` / translation-file impact.
