from os import getenv, system
from sys import exit
from subprocess import check_output
import argparse


def chk(env_var:str, env_var_descr:str) -> bool:
    env_val:str|None = getenv(env_var)
    retv:bool = False
    if env_val is None:
        env_val = "**NOT FOUND**"
    else:
        retv = True
    print(f"{env_var} ({env_var_descr}) -> {env_val}")
    return retv

def env_var_chk(runtime_only:bool) -> bool:
    missing:int = 0
    missing_env = []
    required_env_vars = {
        "LD_LIBRARY_PATH": "path for shared libraries",
        "HOME_TMP_DIR": "local temp dir some library or app may use",
        "HOME_DB_PATH": "DB Path a library or app uses an embedded DB like sqlite",
        "TANUPROJ_HOME": "Directory where tanuapp/lib projects are located",
        "NEKOKAN_BIN_DIR": "Directory where Nekokan Apps are installed",
        "NEKOKAN_LIB_DIR": "Directory where nekokan libraries are located",
        "NEKOKAN_CONF_DIR": "Directory where nekokan configurations are stored",
        "NEKOKAN_CMD_DIR": "Directory where nekokan commands are stored",
        "NEKORC_PATH": "Directory where nekokan app/lib's resources are stored",
        "TANULIB_CPP_CODE_DIR": "Directory where tanulib cpp lib's code is located",
        "NEKOKAN_CODE_DIR": "Direcotory where nekokan code is located",
        "NEKOKAN_ADMIN_USER": "Admin user of nekokan",
        "NEKOKAN_ADMIN_GROUP": "Admin group of nekokan",
        "LOCAL_EMBEDDING_MODEL_DIR": "direcory where embedding model for local use is located",
        "PYTHON_PIP_BIN": "pip command name",
        "LANGSMITH_API_KEY": "Langsmith API key",
        "GITHUB_USER_NAME": "github user name",
        "SERP_API_KEY": "Serp API Key",
        "TAVILY_API_KEY": "Tabily API Key",
        "COHERE_API_KEY": "Cohere API Key"
    }
    if runtime_only:
        del required_env_vars["TANULIB_CPP_CODE_DIR"]
        del required_env_vars["TANUPROJ_HOME"]
        del required_env_vars["NEKOKAN_CODE_DIR"]

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
    nekokan_lib_dir:str|None = getenv("NEKOKAN_LIB_DIR")
    print("checking LD_LIBRARY_PATH contains NEKOKAN_LIB_DIR... Otherwise cpp binaries are likely to fail")
    if ld_lib_path is not None and nekokan_lib_dir is not None:
        libpath_chk_rez = ld_lib_path.find(nekokan_lib_dir) != -1
        print(f"LD_LIBRARY_PATH is {ld_lib_path}. NEKOKAN_LIB_DIR is {nekokan_lib_dir} - {'GOOD' if libpath_chk_rez else 'NG!'}")
        return libpath_chk_rez
    else:
        if ld_lib_path is None:
            print("LD_LIBRARY_PATH is missing")
        if nekokan_lib_dir is None:
            print("TANULIB_DIR is missing")
        return False

def proj_and_nekokan_lib_path_chk() -> bool:
    tlib_cpp_path:str|None = getenv("TANULIB_CPP_CODE_DIR")
    nekokan_path:str|None = getenv("NEKOKAN_CODE_DIR")
    tanuproj_home_path:str|None = getenv("TANUPROJ_HOME")
    print("checking TANULIB_CPP_CODE_DIR and NEKOKAN_CODE_DIR contains TANUPROJ_HOME for integrity ... ");
    if tlib_cpp_path is not None and nekokan_path is not None and tanuproj_home_path is not None:
        chk_rez = tlib_cpp_path.find(tanuproj_home_path) != -1
        if chk_rez:
            chk_rez = nekokan_path.find(tanuproj_home_path) != -1
        print(f"TANULIB_CPP_CODE_DIR is {tlib_cpp_path}, NEKOKAN_CODE_DIR is {nekokan_path} and TANUPROJ_HOME is {tanuproj_home_path} - {'GOOD' if chk_rez else 'NG!'}")
        return chk_rez
    else:
        if tlib_cpp_path is None:
            print("TANULIB_CPP_CODE_DIR missing")
        if nekokan_path is None:
            print("NEKOKAN_CODE_DIR is missing")
        if tanuproj_home_path is None:
            print("TANUPROJ_HOME is missing")
        return False

def compiler_existance_check() -> bool:
    print("checking g++ compiler existance and its version..")
    try:
        check_output(["which", "g++-13"])
        print("found g++-13 or higher - GOOD")
        return True
    except:
        pass

    gpp = check_output(["which", "g++"])
    if len(gpp) != 0:
        try:
            gpp_ver = check_output(["g++", "--version"], text=True)
        except:
            print("seems no g++ exists.")
            return False
        main_ver = int(gpp_ver.split("\n")[0].split(" ")[-1].split(".")[0])
        print(f"g++ version is {main_ver}. This version number must be more than equal to 13.")
        return main_ver >= 13

    print("cannot pass g++ compiler check test - NG")
    return False 

def third_party_lib_install_check():
    print("checking 3rd party library existance..")
    third_parties = set([
        "libcppunit",
        "libcppunit-dev",
        "libpqxx",
        "libpqxx-dev",
        "libcpprest-dev",
        "libcpprest",
        "sqlite3",
        "libsqlite3-dev",
        "libcurl4-openssl-dev"
    ])

    found = set([])
    missing = set([])

    for third_party in third_parties:
        installed_packages = list(filter(lambda x:x.startswith(third_party), check_output(["apt", "list", "--installed"], text=True).splitlines()))
        for package in installed_packages:
            if not package.startswith(third_party + "-dev"):
                found.add(third_party)
        
    missing = third_parties.difference(found)
    print("--- found libraries installed ---")
    buf = ""
    for found_pkg in found: 
        buf += f"{found_pkg},"
    if len(buf) == 0:
        buf = "No dependent libraries have been installed!"
    else:
        buf = buf[:-1]
    print(buf)
    
    print("--- missing libraries ---")
    buf = ""
    for missing_pkg in missing: 
        buf += f"{missing_pkg},"
    if len(buf) == 0:
        buf = "No missing packages - GOOD"
    else:
        buf = buf[:-1]
    print(buf)
    return len(missing) == 0

if __name__ == "__main__":

    parser = argparse.ArgumentParser(prog = "envchk.py")
    parser.add_argument("-r", "--runtime_only", action = "store_true", help="no check for code-dev-related env vars", default = False)
    cargs = parser.parse_args()
    
    if cargs.runtime_only:
        all_good = all([
            env_var_chk(cargs.runtime_only), 
            ld_lib_path_chk(),
            third_party_lib_install_check()
        ])
    else:
        all_good = all([
            env_var_chk(cargs.runtime_only), 
            ld_lib_path_chk(), 
            proj_and_nekokan_lib_path_chk(),
            compiler_existance_check(),
            third_party_lib_install_check()
        ])
    if all_good:
        print("ALL SET!")
    else:
        print("Env set up is not complete.")
    exit(0 if all_good else 1)

