from os import getenv 
from pathlib import Path

module_name = "SAMPLE"


def get_resource_dir() -> str:

    maybe_rc_dir = getenv("NEKORC_PATH")
    if maybe_rc_dir is None:
        raise RuntimeError("NEKORC_PATH must be set")
    return maybe_rc_dir



def main():
    global module_name
    install_dest_base_path = str(Path(get_resource_dir()) / module_name)
    print(f"publishing resource for the project {install_dest_base_path} [module:{module_name}] ... ")


    print(f"publishing resource for the project {install_dest_base_path} done successfully.")


if __name__ == "__main__":
    main()
