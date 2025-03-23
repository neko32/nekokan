from os import getenv
from sys import exit


def chk(env_var:str, env_var_descr:str) -> bool:
    env_val:str|None = getenv(env_var)
    retv:bool = False
    if env_val is None:
        env_val = "**NOT FOUND**"
    else:
        retv = True
    print(f"{env_var} ({env_var_descr}) -> {env_val}")
    return retv

def env_var_chk() -> bool:
    missing:int = 0
    missing_env = []
    required_env_vars = {
        "LD_LIBRARY_PATH": "path for shared libraries",
        "HOME_TMP_DIR": "local temp dir some library or app may use",
        "HOME_DB_PATH": "DB Path a library or app uses an embedded DB like sqlite",
        "TANUAPP_DIR": "Directory where Tanu Apps are installed",
        "TANULIB_DIR": "Directory where tanu libraries are located",
        "TANULIB_CONF_DIR": "Directory where Tanulib configurations are stored",
        "TANULIB_CPP_CODE_DIR": "Directory where tanulib cpp lib's code is located",
        "NEKOKAN_CODE_DIR": "Direcotory where nekokan code is located",
    }
    for (env_var_name, env_var_descr) in required_env_vars.items():
        found = chk(env_var_name, env_var_descr)
        if not found:
            missing = missing + 1
            missing_env.append(env_var_name)

    ret_code = True
    print("Env variables check done. ")
    if missing > 0:
        ret_code = False
        if missing == 1:
            msg = "Missing env variable is:"
        else:
            msg = "Missing env variables are as follows:"
        print(msg)
        missing_env.sort()
        for env_var in missing_env:
            print(f" - {env_var}")
    else:
        print("All required env variables are found successfully.")        
    
    return ret_code

def ld_lib_path_chk() -> bool:
    ld_lib_path:str|None = getenv("LD_LIBRARY_PATH")
    tanulib_dir:str|None = getenv("TANULIB_DIR")
    print("checking LD_LIBRARY_PATH contains TANULIB_DIR. Otherwise cpp binaries are likely to fail")
    if ld_lib_path is not None and tanulib_dir is not None:
        libpath_chk_rez = ld_lib_path.find(tanulib_dir) != -1
        print(f"LD_LIBRARY_PATH is {ld_lib_path}. TANULIB_DIR is {tanulib_dir} - {'GOOD' if libpath_chk_rez else 'NG!'}")
        return libpath_chk_rez
    else:
        if ld_lib_path is None:
            print("LD_LIBRARY_PATH is missing")
        if tanulib_dir is None:
            print("TANULIB_DIR is missing")
        return False

if __name__ == "__main__":
    all_good = all([env_var_chk(), ld_lib_path_chk()])
    if all_good:
        print("ALL SET!")
    else:
        print("Env set up is not complete.")
    exit(0 if all_good else 1)
