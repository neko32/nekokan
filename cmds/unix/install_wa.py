from argparse import ArgumentParser
import os
import requests
import shutil
from pathlib import Path

class Catalog:
    def __init__(self, app_provider:str, app_name:str, version:str):
        self.app_provider = app_provider
        self.app_name = app_name
        self.version = version
        self.app_catalog = {
            "neko32/nekotimer": ["wa/nekotimer-frontend.tar.gz", "srv/nekotimer-backend"]
        }

    def fetch_and_install(self) -> None:
        try:
            base_url = f"https://github.com/{self.app_provider}/{self.app_name}/releases/download/{self.version}"
            for asset_pack in self.app_catalog.get(f"{self.app_provider}/{self.app_name}", []):
                asset = asset_pack.split("/")
                asset_type = asset[0]
                asset_name = asset[1]
                asset_url = f"{base_url}/{asset_name}"
                print(f"Fetching {asset_url}...")

                headers = {
                # pretending wget
                    "User-Agent": "Wget/1.21.1" 
                }

                resp = requests.get(asset_url, headers=headers, stream=True, allow_redirects=True)
                resp.raise_for_status()

                tmp_path = os.path.join("/tmp", asset_name)
                with open(tmp_path, "wb") as f:
                    for chunk in resp.iter_content(chunk_size=8192):
                        f.write(chunk)
                print(f"Downloaded {asset_name} to {tmp_path}") 

                if asset_type == "wa":                
                    self.install_wa(self.app_name, asset_name, self.version, tmp_path)
                elif asset_type == "srv":
                    self.install_srv(self.app_name, asset_name, self.version, tmp_path)

        except Exception as e:
            print(f"Error fetching and installing: {e}")
            raise

    def install_wa(self, app_name:str, asset_name:str, version:str, tmp_path:str) -> None:
        install_path = Path(os.path.join("/opt", "waapp", f"{app_name}_{version}"))
        if Path(install_path).exists():
            shutil.rmtree(install_path)
        install_path.mkdir(parents=True, exist_ok=True)
        shutil.move(tmp_path, install_path)
        shutil.unpack_archive(str(install_path / asset_name), str(install_path))

        slink_asset = ""

        if asset_name.endswith(".tar.gz"):
            slink_asset = Path(asset_name).with_suffix("").with_suffix("")
        else:
            slink_asset = asset_name

        slink_path = Path(os.path.join("/opt", "waapp", slink_asset))

        print(f"Installing {app_name} wa to {install_path / "dist" / "wa"} as slink {slink_path}")

        if slink_path.exists():
            slink_path.unlink()
        slink_path.symlink_to(install_path / "dist" / "wa", target_is_directory=True)
        print(f"Installed {app_name} wa to {install_path} as slink {slink_path}")

    def install_srv(self, app_name:str, asset_name:str, version:str, tmp_path:str) -> None:
        install_path = Path(os.path.join("/opt", "srv", f"{app_name}_{version}"))
        if Path(install_path).exists():
            shutil.rmtree(install_path)
        install_path.mkdir(parents=True, exist_ok=True)
        shutil.move(tmp_path, install_path / asset_name)

        slink_path = Path(os.path.join("/opt", "srv", asset_name))
        if slink_path.exists():
            slink_path.unlink()
        slink_path.symlink_to(install_path / asset_name)

        print(f"Installed {app_name} srv to {install_path} as slink {slink_path}")


def main() -> int:
    try:
        parser = ArgumentParser(description="Install wa")
        parser.add_argument("--app_provider", default="neko32")
        parser.add_argument("--app-name", required = True)
        parser.add_argument("--version", required = True)
        args = parser.parse_args()
    except Exception as e:
        print(f"Error parsing arguments: {e}")
        return 1

    catalog:Catalog = Catalog(args.app_provider, args.app_name, args.version)
    try:
        catalog.fetch_and_install()
        return 0
    except Exception as e:
        print(f"Error fetching and installing: {e}")
        return 1

if __name__ == "__main__":
    exit(main())

