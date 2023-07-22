import argparse


def run_main():
    parser = argparse.ArgumentParser(description="Compiles a *.jth file into a CHIP-8 binary file.")
    parser.add_argument("file", type=str, help="Path to the *.jth file to compile.")
    parser.add_argument("out", type=str, help="Path to the generated CHIP-8 binary file.")
    args = parser.parse_args()
    
    print("Reading", args.file)
    with open(args.file) as f:
        source_code = f.readlines()
    
    print("Writing to", args.out)
    with open(args.out, mode="wb") as f:
        for line in source_code:
            operator, number = line.strip().split(" ")
            if operator == "EMIT":
                base = 16 if "0x" in number else 10
                # print(f"> Number: {number} | Base: {base}")
                num = int(number, base=base)
                f.write(bytes([num]))
            else:
                print("Unknown operator", operator)
                exit(-1)

if __name__ == "__main__":
    run_main()
