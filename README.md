# Midterm Project: Implement ls(1)

- Student: Nguyen Minh Hieu
- Student ID: 24IT065
- Repository: https://github.com/deptraibanfifai/NguyenMinhHieu_24IT065_midterm

## 1. Project overview

This project implements a simplified UNIX ls command in C,
based on the provided NetBSD ls(1) manual.

The program uses filesystem APIs directly, including opendir,
readdir, lstat, stat and readlink. It does not call the system
ls command to produce its listings.

## 2. Implemented features

- List the current directory when no operands are given.
- Accept regular files and multiple file/directory operands.
- Display non-directory operands before directory contents.
- Sort files and directory operands separately.
- Print one entry per line by default.
- Display directory headings for multiple operands or recursion.
- Report errors to standard error.
- Return 0 on success and a nonzero status on error.

Supported options:

| Option | Behavior |
|---|---|
| -A | Include hidden entries except . and ..; automatic for root |
| -a | Include all directory entries |
| -c | Select file status-change time |
| -d | Display directory operands themselves |
| -F | Append file-type indicators |
| -f | Disable sorting |
| -h | Display human-readable sizes |
| -i | Display inode numbers |
| -k | Display block counts in kilobytes |
| -l | Display long format |
| -n | Display long format with numeric UID/GID |
| -q | Replace non-printable filename bytes with ? |
| -R | Recursively list subdirectories |
| -r | Reverse the selected sort order |
| -S | Sort by size, largest first |
| -s | Display allocated filesystem blocks |
| -t | Sort by the selected timestamp, newest first |
| -u | Select access time |
| -w | Print raw filename bytes |

The last option wins for -l/-n, -c/-u, -q/-w,
-h/-k and -d/-R.

Child symbolic links are not followed during recursion.
The entries . and .. are never recursively visited.

BLOCKSIZE controls block-count units when neither -h nor -k
is selected. TZ controls the timezone used for date output.

## 3. Source organization

| File | Responsibility |
|---|---|
| src/main.c | Program entry point |
| src/options.c | Command-line parsing |
| src/entry.c | Entry storage and directory reading |
| src/list.c | Hidden-entry filtering and directory listing |
| src/operands.c | Multiple operands and recursive traversal |
| src/sort.c | Sorting |
| src/print.c | Short and long output |
| src/util.c | Permissions, sizes, dates and type indicators |
| include/*.h | Shared types and module interfaces |

## 4. Build and run

Requirements: a C99 compiler and make on NetBSD.

```sh
make
./my_ls
./my_ls -la
./my_ls -lh src
./my_ls -R include src
./my_ls -d src
./my_ls Makefile src
./my_ls -n Makefile
BLOCKSIZE=1024 ./my_ls -s src
TZ=UTC ./my_ls -l src

## Verified test results on NetBSD

The program compiled successfully with:
-Wall -Wextra -Werror -std=c99

Verified scenarios:
- Long format, numeric IDs and column alignment.
- Modification, access and status-change timestamps.
- Inode and allocated-block columns.
- Files and multiple directory operands.
- Recursive listing, including -aR.
- Hidden entries, symbolic links and dangling links.
- FIFO files, setuid permissions and filenames with spaces.
- BLOCKSIZE=2048 with -s, -ls, -ks and -lks.
- Last-option precedence for -h/-k.
- Replacement of a newline in a filename with ? using -q.
- Permission errors and invalid options return exit status 1.
- Valid operands are listed even when another operand is invalid.

Known differences from the installed /bin/ls:
- -nl uses owner/group names because the final -l wins,
  as specified by the supplied manual.
- -dR enables recursion because the final -R wins.
- -f disables sorting without implicitly enabling -a.
- Human-readable directory totals use allocated blocks
  (st_blocks multiplied by 512), rather than summed file sizes.

These results apply to the tested scenarios.
