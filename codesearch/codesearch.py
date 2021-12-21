import argparse
import contextlib
import json
import os
from collections import defaultdict
from typing import Dict, List, Optional, Set


def main_make_index(directory: str, *, index_file: str) -> None:
    indexer = Indexer(index_file)
    indexer.make_index(directory)
    indexer.save()


def main_search(query: str, *, index_file: str) -> None:
    searcher = Searcher(index_file)
    paths = searcher.search(query)
    for path in paths:
        print(path)


class Indexer:
    index_file: str
    index: "Index"

    def __init__(self, index_file: str) -> None:
        self.index_file = index_file
        self.index = Index()

    def make_index(self, directory: str) -> None:
        self._index_file_or_directory(directory)

    def save(self) -> None:
        self.index.save(self.index_file)

    def _index_file_or_directory(self, path: str) -> None:
        if self.should_ignore_path(path):
            return

        if is_directory(path):
            for subpath in list_paths_in_directory(path):
                self._index_file_or_directory(os.path.join(path, subpath))
        else:
            self._index_file(path)

    def _index_file(self, path: str) -> None:
        with contextlib.suppress(UnicodeDecodeError, FileNotFoundError):
            with open(path, "r", encoding="utf8") as f:
                for word in f.read().split():
                    if word.isalpha():
                        self.index.add_word(word, path)

    def should_ignore_path(self, path: str) -> bool:
        basename = os.path.basename(path)
        if basename == ".venv" or basename == ".git":
            return True

        return False


class Searcher:
    index: "Index"

    def __init__(self, index_file: str) -> None:
        self.index = Index.load_from_file(index_file)

    def search(self, query: str) -> List[str]:
        words = self.index.words()
        return list(words[query])


class Index:
    def __init__(self, word_to_path: Optional[Dict[str, Set[str]]] = None) -> None:
        self.word_to_path = word_to_path or defaultdict(set)

    @classmethod
    def load_from_file(cls, path: str) -> "Index":
        with open(path, "r", encoding="utf8") as f:
            d = json.load(f)

        word_to_path = defaultdict(set)
        for key, value in d.items():
            word_to_path[key] = set(value)

        return cls(word_to_path)

    def save(self, path: str) -> None:
        j = {}
        for key, value in self.word_to_path.items():
            j[key] = list(value)

        with open(path, "w", encoding="utf8") as f:
            json.dump(j, f)

    def add_word(self, word: str, path: str) -> None:
        self.word_to_path[word].add(path)

    def words(self) -> Dict[str, Set[str]]:
        return self.word_to_path


def is_directory(path: str) -> bool:
    return os.path.isdir(path)


def list_paths_in_directory(directory: str) -> List[str]:
    return os.listdir(directory)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Search indexed source code.")
    subparsers = parser.add_subparsers(help="Subcommands")

    parser_index = subparsers.add_parser("index", help="Create a search index")
    parser_index.add_argument("directory")
    parser_index.add_argument("--out", required=True)
    parser_index.set_defaults(subcommand="index")

    parser_search = subparsers.add_parser("search", help="Search for something")
    parser_search.add_argument("query")
    parser_search.add_argument("--index", required=True)
    parser_search.set_defaults(subcommand="search")

    args = parser.parse_args()
    if args.subcommand == "index":
        main_make_index(args.directory, index_file=args.out)
    elif args.subcommand == "search":
        main_search(args.query, index_file=args.index)
    else:
        raise Exception(f"unknown subcommand: {args.subcommand}")
