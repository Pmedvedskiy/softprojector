# Lessons

## 2026-05-30 — Build needs Qt 6.4+, not Ubuntu 22.04's apt Qt 6.2
**Rule:** Build with Qt 6.4+ (currently using 6.8.3 from `aqt` installed at `~/Qt/6.8.3/gcc_64`). Don't try Ubuntu 22.04's `qt6-base-dev` (6.2.4).
**Why:** Upstream commit `2c8f9a9 "Migrage few text parsing items"` added `using namespace Qt::StringLiterals;` and `"..."_L1` literals in `src/sources/managedatadialog.cpp`. Both require Qt 6.4. Qt 6.2 fails with `'StringLiterals' is not a namespace-name` and `unable to find string literal operator 'operator""_L1'`.
**How to apply:** When setting up a build env (or telling someone else how to), use `aqt install-qt linux desktop 6.8.3 linux_gcc_64 -O ~/Qt -m qtmultimedia`. Invoke as `~/Qt/6.8.3/gcc_64/bin/qmake6` and run with `LD_LIBRARY_PATH=~/Qt/6.8.3/gcc_64/lib QT_PLUGIN_PATH=~/Qt/6.8.3/gcc_64/plugins`.

## 2026-05-30 — Vendored `qmediaplaylist.h` needs a sibling `qmediaenumdebug.h`
**Rule:** `3rdparty/headers/qmediaplaylist.h` includes `qmediaenumdebug.h`, which Qt removed in 6.5+. A vendored copy lives next to it in `3rdparty/headers/qmediaenumdebug.h`; the include is the quoted form `"qmediaenumdebug.h"`.
**Why:** `qmediaenumdebug.h` was a Qt private QtMultimedia header dropped when `QMediaPlaylist` was removed in Qt 6. The vendored `QMediaPlaylist` still expands `Q_MEDIA_ENUM_DEBUG` against its own enums. Nothing in the project actually streams those enums to qDebug, so the macro could also be made a no-op — but keeping the original definition keeps the vendored Qt code unchanged in spirit.
**How to apply:** If `qmediaplaylist.h` ever gets resynced from upstream Qt, re-vendor `qmediaenumdebug.h` from a Qt ≤6.4 source tree and keep the include quoted.

## 2026-05-30 — Qt5→Qt6 multimedia migration is incomplete in the media widgets
**Rule:** `MediaWidget`, `MediaControl`, `ProjectorDisplayScreen` still wire up Qt5-era `QMediaPlayer` signals/slots (`stateChanged`, `volumeChanged`, `error(Error)`, `setVolume`, `setMuted`, `videoAvailableChanged`). Connects fail silently at runtime.
**Why:** In Qt 6 `QMediaPlayer` lost volume/muting (moved to `QAudioOutput`), renamed `stateChanged → playbackStateChanged(PlaybackState)`, renamed `error → errorChanged(Error,QString)`, removed `videoAvailableChanged`. The upstream Qt6 migration PR landed the type changes but not all the connect call sites.
**How to apply:** Don't rely on the in-app media player working until these are migrated. If asked to fix media playback, the broken connect calls are visible at startup in stderr — fix them at the `connect(...)` call sites in `mediawidget.cpp`, `mediacontrol.cpp`, `projectordisplayscreen.cpp`. Check whether upstream has fixed them first to avoid merge conflicts.

## 2026-05-30 — Don't commit design/spec docs, only commit features
**Rule:** Do not `git commit` brainstorming design specs or planning docs. Commit only actual feature/code changes.
**Why:** User stopped a commit of a design spec saying "just commit features." Specs can live on disk as working notes but stay out of git history.
**How to apply:** When a workflow (e.g. the brainstorming skill) says to commit a design doc, skip the commit step. Write the spec file if useful, then go straight to implementing and commit only the code.

## 2026-09-01 - `gh` resolves to the upstream repo here; always pass `-R`
**Rule:** In this checkout, run every `gh` command with `-R pmedvedskiy/softprojector` (or rely on `git config remote.origin.gh-resolved base`, set on 2026-09-01; the installed gh predates `gh repo set-default`), and test-compile Windows-only code with `gh workflow run windows-installer.yml --ref pmedvedskiy/customizations -f version=<ver>-rc1` before pushing a `v*` tag.
**Why:** With both `origin` and `upstream` remotes present, `gh` prefers `upstream`, so `gh release list` / `gh run list` silently showed SoftProjector/softprojector (no v2.2.1 release, only upstream Linux runs) and looked like the fork's release was missing. Separately, `#ifdef Q_OS_WIN` code cannot be compiled from WSL2, and a broken tag build would need a re-tag; the manual dispatch with a branch ref works even though the workflow is not on `master`.
**How to apply:** Any time you query releases, runs, or trigger workflows for this fork; any time a change touches Windows-only code before cutting a release tag.

## 2026-09-01 - Qt 6 containers no longer tolerate bad indexes; test with an empty database
**Rule:** Any `removeAt()` / `at()` / `remove()` whose index comes from a combo box or `indexOf()` must be range-checked (an empty combo reports `currentIndex() == -1`). Before shipping, exercise the app against a freshly created `spData.sqlite` (no bibles, no songs, no themes), not just the populated one in `src/unix_build/bin`.
**Why:** Upstream code relied on Qt 5's `QList::removeAt()` silently ignoring out-of-range indexes. Qt 6 erases unchecked in release builds, so Settings crashed on a fresh install (`BibleSettingWidget::updateSecondaryBibleMenu`, fixed in 9265cb8 / v2.2.3). The church PC never showed it because it has bibles installed.
**How to apply:** When touching or reviewing code migrated from Qt 5, grep for `removeAt(`, `.at(`, `[` with combo-box or `indexOf` indexes. To reproduce GUI crashes from WSL2 without click automation: copy the binary alone into an empty dir (fresh DB), run under `gdb -batch` with `-platform offscreen`, break in `SoftProjector::positionDisplayWindow` to grab `this` (store it in a variable NOT named `$sp` / `$pc` / `$fp`, those are gdb registers), break on `QApplication::exec`, then `call ((SoftProjector*)$var)->on_actionSettings_triggered()`; a crash gives a backtrace, a timeout means the dialog opened. Build with `CONFIG+=force_debug_info` and `make -B` (make does not rebuild on flag changes) for line numbers; add `-fsanitize=address` for memory errors.

## 2026-10-06 - QRegularExpression needs UseUnicodePropertiesOption for non-Latin text
**Rule:** Every `QRegularExpression` that uses `\w`, `\W` or `\b` on user or verse text must set `QRegularExpression::UseUnicodePropertiesOption` (combined with `CaseInsensitiveOption` where that is used).
**Why:** Upstream's Qt 5 to 6 migration (commit b405594) swapped `QRegExp` for `QRegularExpression`, whose word classes are ASCII-only by default. The shared `clean()` in `src/sources/song.cpp` wiped Cyrillic search text to an empty string, so Bible search answered "Please enter search text" for Russian input, and word-boundary searches never matched. Fixed on 2026-10-06 in `clean()`, the Bible widget, `Bible::searchBible` and the result highlighter. The two song-search regexes in `songwidget.cpp` (whole-word and all-words) still lack the option.
**How to apply:** When adding or touching any regex over text, add the option. Regression tests live in `tests/search/` (build and run steps in the `.pro` header); run them after touching search code.

## 2026-10-06 - Upstream's Qt 6 migration commits broke behaviour silently; run the test projects after every sync
**Rule:** After merging upstream, build and run every project under `tests/` (search, layout, wheelguard) before trusting the app, and treat any upstream commit titled "migrate" or "Migrage" as suspect.
**Why:** Two upstream migration commits changed semantics without changing behaviour visibly in English: b405594 made the search regexes ASCII-only (see the entry above), and 2c8f9a9 replaced the hex decode in `Settings::textToByt` with `text.toUtf8()`, so no saved splitter layout (including the Bible column widths added in May) was ever restored. The user reported both as "search doesn't work" and "layout doesn't save".
**How to apply:** The test projects pin both behaviours. Build steps are in each `.pro` header; `tests/common.pri` holds the shared source list.

## 2026-10-06 - Testing wheel propagation needs a spontaneous event sent through notify()
**Rule:** In a QtTest that checks whether a wheel event reaches a parent (for example a scroll area), build the `QWheelEvent`, call `QSpontaneKeyEvent::setSpontaneous(&event)` from `<QtTest/qtestspontaneevent.h>`, and dispatch it with `qApp->notify(target, &event)`, never `QApplication::sendEvent`.
**Why:** `QApplication::notify` only walks the parent chain for spontaneous wheel events, and `sendEvent()` clears the spontaneous flag before dispatching. The WheelGuard scroll test failed twice for that reason while the guard itself was correct; a probe logging which widgets received the event showed it.
**How to apply:** Any test around `WheelGuard`, scroll areas, or other event filters that rely on an ignored event propagating. Production code can simply `event->ignore(); return true;` in the filter.

## 2026-10-06 - Display window: no WS_BORDER, OpenGL instead (church-PC result pending)
**Rule:** Do not reintroduce `setHasBorderInFullScreen(true)` on the display window; the 1 px frame shows on the projector. The blackout workaround is now `QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL)` in `main.cpp`.
**Why:** The v2.2.2 border fixed the mixed-refresh-rate blackout (or the user's "Disable fullscreen optimizations" compatibility tick did; they do not remember which) but the border was visible during verses, so it was removed on 2026-10-06. OpenGL cannot be verified from WSL.
**How to apply:** If the blackout comes back on the church PC after this build, first tick "Disable fullscreen optimizations" on SoftProjector.exe (Properties, Compatibility); only if that fails, revert the main.cpp commit and look for another DWM-safe approach.
