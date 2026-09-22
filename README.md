This program works its way into every folder it encounters and renders all the files it touches unusable.

---

## Key Features

- **Recursive Directory Traversal**: Operates on nested folders to reach all target files.
- **In-Place Processing**: Modifies files directly on disk using read-write file streams.
- **Constant Memory Footprint**: Reads and writes files in configurable 8 KB chunks (`BUFFER_SIZE`), enabling it to process multi-gigabyte files efficiently without consuming too much RAM.
- **File System Safety**:
  - Uses `lstat` and stat modes (`S_ISREG`, `S_ISDIR`) to ignore symbolic links, FIFOs, sockets, and character devices—preventing infinite loops or deadlocks.
  - Skips directory self/parent links (`.` and `..`).
  - Guards against path buffer overflows using `snprintf` bounded by `PATH_MAX`.
- **Stream Synchronization**: Explicitly flushes I/O buffers (`fflush`) between read/write pointer shifts to satisfy ISO C stream rules.

---

## How It Works

### 1. Directory Analysis & Filtering
When `encrypt_directory` or `decrypt_directory` runs on a target path:
1. It opens the directory using `opendir` and iterates over entries with `readdir`.
2. Paths are safely constructed using `snprintf(filepath, sizeof(filepath), "%s/%s", folder_path, entry->d_name)`.
3. `analyse_path()` invokes `lstat` to evaluate the file node type:
   - **Type 1 (`S_ISDIR`)**: Triggers a recursive call to process the sub-directory.
   - **Type 2 (`S_ISREG`)**: Triggers file processing (`encrypt_file` or `decrypt_file`) and increments the `file_count` tracker.
   - **Type 0 (Other)**: Ignores symlinks, special files, or unreadable paths.

### 2. Chunk-Based In-Place Processing
For every regular file encountered:
1. Opens the file handle in `rb+` mode (read/write binary).
2. Captures current file offset using `ftell`.
3. Reads up to `BUFFER_SIZE` bytes into an `unsigned char` memory buffer.
4. Performs an 8-bit unsigned integer shift (addition for encryption, subtraction for decryption) across the buffer array.
5. Invokes `fseek` to rewind the stream pointer back to the captured `position`.
6. Writes the transformed bytes over the original block using `fwrite`.
7. Calls `fflush` to ensure data persists to disk and repositions the file pointer forward for the next iteration.
8. Closes file handle upon reaching EOF (`fread` returns 0).