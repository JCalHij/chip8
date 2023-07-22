import hashlib


BUF_SIZE = 64 * 1024 # [bytes]


def get_md5_for_file(path: str):
    md5 = hashlib.md5()
    with open(path, mode="rb") as f:
        while True:
            data = f.read(BUF_SIZE)
            if not data:
                break
            md5.update(data)
    return md5.hexdigest()


def test_against_breakout_baseline():
    generated_file = "test_breakout.ch8"
    baseline_file = "tests/files/baseline_breakout.ch8"

    assert get_md5_for_file(generated_file) == get_md5_for_file(baseline_file)
