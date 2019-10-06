import argparse
import shutil
import traceback
import urllib.request
from io import BytesIO
from pathlib import Path
import json
from zipfile import ZipFile
import struct

from PIL import Image


def find_textures(assets_dir: Path):
    models_dir = assets_dir / 'minecraft' / 'models' / 'block'

    if not models_dir.exists():
        raise Exception('Cannot find models dir!')

    for file in models_dir.glob('*.json'):
        block_desc = json.loads(file.read_text())

        if 'parent' in block_desc and block_desc['parent'] == 'block/cube_all':
            yield (file.stem, block_desc['textures']['all'])


def create_texture(img: Image):
    img = img.convert('RGBA')

    pixels = img.tobytes()
    width, height = img.size

    return width, height, pixels


def find_and_extract_avgs(assets_dir):
    texture_dir = assets_dir / 'minecraft' / 'textures'
    zip_file = assets_dir / 'minecraft' / 'resourcepacks' / 'programmer_art.zip'

    if texture_dir.exists():
        def load_img(path):
            file_path = texture_dir / path
            with file_path.open('rb') as fp:
                return Image.open(BytesIO(fp.read()))
    else:
        file = ZipFile(zip_file)

        def load_img(path):
            with file.open('assets/minecraft/textures/' + path) as fp:
                return Image.open(fp)

    for name, tex_path in find_textures(assets_dir):
        img = load_img(tex_path + '.png')
        if img.size != (16, 16):
            continue
        yield name, create_texture(img)


def extract_version(version_file):
    with version_file.open('rt') as fp:
        return json.load(fp)['id']


def run_and_print(working_dir):
    return (
        extract_version(working_dir / 'version.json'),
        [('minecraft:' + str(a), b) for a, b in find_and_extract_avgs(working_dir / 'assets')]
    )


def print_json(data, out_file):
    res = {
        'versions': data
    }
    with open(out_file, 'wt') as fp:
        json.dump(res, fp)


def print_bin(data, out_file):
    with out_file.open('wb') as fp:
        version, blocks = data
        # Print mc version
        fp.write(version.encode() + b'\0')

        # Print texture len (unsigned long)
        fp.write(struct.pack('!L', len(blocks)))
        for texture in blocks:
            name, tex = texture
            # Block type (always zero for now)
            fp.write(struct.pack('!B', 0))
            # Block name (cstring)
            fp.write(name.encode() + b'\0')
            # Block data (TODO)
            fp.write(struct.pack('!B', 0))

            assert tex[0] == 16
            assert tex[1] == 16
            assert len(tex[2]) == 16*16*4

            fp.write(tex[2])


def download_version(version, tmp_dir):
    download_url = 'https://api.github.com/repos/InventivetalentDev/minecraft-assets/zipball/' + version

    out_dir = tmp_dir / version

    with urllib.request.urlopen(download_url) as response:
        tmp_file = tmp_dir / (version + '.zip')
        with tmp_file.open('wb') as fp:
            shutil.copyfileobj(response, fp)

        with ZipFile(tmp_file) as zipfile:
            zipfile.extractall(str(out_dir))

        tmp_file.unlink()

        children_dir = list(out_dir.glob('*'))[0]
        for child in children_dir.glob('*'):
            child.rename(child.absolute().parent.parent / child.name)

        children_dir.rmdir()

    return out_dir


def download_version_list():
    url = 'https://launchermeta.mojang.com/mc/game/version_manifest.json'
    with urllib.request.urlopen(url) as response:
        res = json.load(response)

        return [x['id'] for x in res['versions'] if x['type'] == 'release']


def run_folder(folder, out_path):
    version, data = run_and_print(folder)

    print_bin((version, data), out_path / (version + '.bin'))


def run_all(version_list, tmp_path, out_path):
    tmpdir = Path(tmp_path)
    tmpdir.mkdir(parents=True, exist_ok=True)

    for version in version_list:
        print('Running for: ' + version)
        version_dir = None

        try:
            version_dir = download_version(version, tmpdir)
            run_folder(version_dir, out_path)

        except Exception as e:
            print('Error executing version ' + version)
            traceback.print_exception(type(e), e, e.__traceback__)

        finally:
            if version_dir is not None:
                shutil.rmtree(version_dir.absolute())


def run_all_versions(tmp_path, out_path):
    version_list = download_version_list()

    def version_predicate(v: str):
        decoded = tuple(int(x) for x in v.split('.'))
        # Skip all versions before 1.8
        return decoded >= (1, 8)

    version_list = [v for v in version_list if version_predicate(v)]
    run_all(version_list, tmp_path, out_path)


parser = argparse.ArgumentParser()
parser.add_argument('-t', '--tmp-path', help='Folder used to store temporary files', default='mc_tmp')
parser.add_argument('-o', '--output', help='Folder used to store generated files', default='mc_build')

group = parser.add_mutually_exclusive_group()
group.add_argument('-v', '--versions', nargs='+', help='List of versions to download')
group.add_argument('-a', '--all-versions', help='Downloads all versions (default)', action='store_const', const=True, default=False)
group.add_argument('-f', '--folder', help='Uses an already downloaded folder')


def main():
    args = parser.parse_args()

    tmp_path = Path(args.tmp_path)
    out_path = Path(args.output)

    tmp_path.mkdir(parents=True, exist_ok=True)
    out_path.mkdir(parents=True, exist_ok=True)

    if args.versions:
        print('Compiling versions: ' + ','.join(args.versions))
        run_all(args.versions, tmp_path, out_path)
    elif args.folder:
        folder = Path(args.folder)

        if folder.exists():
            print('Error: Cannot find folder' + args.folder)
            exit(-1)

        print('Using folder ' + args.folder)
        run_folder(folder, out_path)
    else:
        print('Compiling all versions')
        run_all_versions(tmp_path, out_path)


if __name__ == '__main__':
    main()

