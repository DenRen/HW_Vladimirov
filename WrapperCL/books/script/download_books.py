#!/usr/bin/python3

import urllib.request
from tqdm import tqdm

class DownloadProgressBar(tqdm):
    def update_to(self, b=1, bsize=1, tsize=None):
        if tsize is not None:
            self.total = tsize
        self.update(b * bsize - self.n)


def download_url(url, output_path):
    with DownloadProgressBar (unit='B', unit_scale=True, miniters=1,
                              desc=url.rsplit('/', maxsplit = 1)[-1]) as t:
        urllib.request.urlretrieve(url, filename=output_path, reporthook=t.update_to)

def download_book (nameFileLinks):
    with open (nameFileLinks, "r") as fileLinks:
        links = fileLinks.readlines ();

        for link in links:
            link = link.strip ()
            nameFile = '../' + link.rsplit ('/', maxsplit=1)[-1]
            
            download_url (link, nameFile)
        
nameFileLinks = 'links.txt'
download_book (nameFileLinks)