import os
import re
import json
import sys

def extract_back_sections_from_file(filepath: str) -> str:
    with open(filepath, 'r', encoding='utf-8') as f:
        code = f.read()
    matches = re.findall(r'/\*back_start\*/(.*?)/\*back_end\*/', code, re.DOTALL)
    return '\n'.join(matches)

def deep_set(d, keys, value):
    """递归设置嵌套键值，如 keys=["size", "x"]"""
    for key in keys[:-1]:
        if key not in d or not isinstance(d[key], dict):
            d[key] = {}
        d = d[key]
    d[keys[-1]] = value

def parse_macros(code: str) -> dict:
    # 支持 MKMLxxx 或 MKMLa_b_c 类型
    macro_pattern = re.compile(r'#ifndef\s+(MKML(\w+))\s+#define\s+\1\s*(?:"?(.*?)"?)?\s+#endif', re.DOTALL)
    result = {}

    for match in macro_pattern.finditer(code):
        full_macro, body, value = match.group(1), match.group(2), match.group(3)
        value = value.strip('"') if value else True

        # 拆分标签（如 size_x_y → ["size", "x", "y"]）
        keys = body.split('_')
        keys = [k.lower() for k in keys]
        deep_set(result, keys, value)

    return result

def generate_html(data: dict, level=0) -> str:
    html = ""
    indent = "    " * level
    if isinstance(data, dict):
        for key, val in data.items():
            if isinstance(val, dict):
                html += f"{indent}<h{level+2}>{key}</h{level+2}>\n"
                html += f"{indent}<div style='margin-left:20px;'>\n{generate_html(val, level+1)}{indent}</div>\n"
            else:
                html += f"{indent}<p><b>{key}</b>: {val}</p>\n"
    return html

def collect_back_info(root_dir: str) -> dict:
    all_macros = {}

    for dirpath, _, filenames in os.walk(root_dir):
        for fname in filenames:
            if fname.endswith(('.cpp', '.h')):
                path = os.path.join(dirpath, fname)
                print(f"[mkcc_doc] Parsing: {path}")
                back_code = extract_back_sections_from_file(path)
                macros = parse_macros(back_code)

                def merge_dicts(a, b):
                    for k, v in b.items():
                        if k in a and isinstance(a[k], dict) and isinstance(v, dict):
                            merge_dicts(a[k], v)
                        else:
                            a[k] = v

                merge_dicts(all_macros, macros)

    return all_macros

def main():
    project_dir = sys.argv[1] if len(sys.argv) > 1 else "core"
    print(f"[mkcc_doc] Scanning directory: {project_dir}")

    macros = collect_back_info(project_dir)

    if not macros:
        print("[mkcc_doc] No back section macros found.")
        return

    output_dir = "docs"
    os.makedirs(output_dir, exist_ok=True)

    json_path = os.path.join(output_dir, "mkcc_doc.json")
    with open(json_path, 'w', encoding='utf-8') as f:
        json.dump(macros, f, indent=2, ensure_ascii=False)
    print(f"[mkcc_doc] JSON saved to {json_path}")

    html_path = os.path.join(output_dir, "mkcc_doc.html")
    html = f"""<!DOCTYPE html>
<html><head><meta charset="UTF-8"><title>MKCC Doc</title></head><body>
<h1>MKCC Documentation</h1>
<h2>head:</h2>
{generate_html(macros)}
</body></html>"""
    with open(html_path, 'w', encoding='utf-8') as f:
        f.write(html)
    print(f"[mkcc_doc] HTML saved to {html_path}")

if __name__ == "__main__":
    main()
