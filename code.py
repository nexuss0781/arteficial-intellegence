import os

output_file = "codes.md"

def dump_files(base_path):
    with open(output_file, "w", encoding="utf-8") as out:
        for root, dirs, files in os.walk(base_path):
            for fname in files:
                path = os.path.join(root, fname)
                try:
                    with open(path, "r", encoding="utf-8", errors="ignore") as f:
                        out.write("File: " + path + "\n")
                        out.write("----------------------------------------\n")
                        out.write(f.read() + "\n\n")
                except:
                    pass

if __name__ == "__main__":
    dump_files(".")
