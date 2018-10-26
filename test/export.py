#!/usr/bin/python3

import pandas as pd

for name, dt in pd.read_excel("Region.xlsx", dtype="object", sheet_name=None).items():
	if name == "center":
		continue
	dt.to_csv("input/{}.tsv".format(name), sep="\t", index=False)
