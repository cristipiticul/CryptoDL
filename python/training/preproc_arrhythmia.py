#!/usr/bin/python3
"""
Handles the absent data cells marked with "?".
If one column has more than 10 missing cells, the column is dropped.
If it has fewer than 10 missing cells, the missing values are replaced with the average of that column.
Also normalizes the values.
Also, subtracts 1 from the class number so that it's easier to map to one-hot-encoding.

Prerequisites: download the arrhythmia.data from here https://archive.ics.uci.edu/ml/datasets/Arrhythmia
into the ../../datasets/arrhythmia/ folder"
"""
import pandas as pd

IN_FILE = "../../datasets/arrhythmia/arrhythmia.data"
OUT_FILE = "../../datasets/arrhythmia/arrhythmia_preproc.data"

names = [
    "Age",
    "Sex",
    "Height",
    "Weight",
    "QRS duration",
    "P-R interval",
    "Q-T interval",
    "T interval",
    "P interval",
    "QRS",
    "T",
    "P",
    "QRST",
    "J",
    "Heart rate",
    "Q wave",
    "R wave",
    "S wave",
    "R' wave",
    "S' wave",
    "Number of intrinsic deflections",
    "Existence of ragged R wave",
    "Existence of diphasic derivation of R wave",
    "Existence of ragged P wave",
    "Existence of diphasic derivation of P wave",
    "Existence of ragged T wave",
    "Existence of diphasic derivation of T wave",
    *["DII{}".format(i) for i in range(1, 13)],
    *["DIII{}".format(i) for i in range(1, 13)],
    *["AVR{}".format(i) for i in range(1, 13)],
    *["AVL{}".format(i) for i in range(1, 13)],
    *["AVF{}".format(i) for i in range(1, 13)],
    *["V1_{}".format(i) for i in range(1, 13)],
    *["V2_{}".format(i) for i in range(1, 13)],
    *["V3_{}".format(i) for i in range(1, 13)],
    *["V4_{}".format(i) for i in range(1, 13)],
    *["V5_{}".format(i) for i in range(1, 13)],
    *["V6_{}".format(i) for i in range(1, 13)],
    *["{} wave 2".format(i) for i in ["JJ", "Q", "R", "S", "R'", "S'", "P", "T"]],
    "QRSA",
    "QRSTA",
    *["DII2_{}".format(i) for i in range(1, 11)],
    *["DIII2_{}".format(i) for i in range(1, 11)],
    *["AVR2_{}".format(i) for i in range(1, 11)],
    *["AVL2_{}".format(i) for i in range(1, 11)],
    *["AVF2_{}".format(i) for i in range(1, 11)],
    *["V1_2_{}".format(i) for i in range(1, 11)],
    *["V2_2_{}".format(i) for i in range(1, 11)],
    *["V3_2_{}".format(i) for i in range(1, 11)],
    *["V4_2_{}".format(i) for i in range(1, 11)],
    *["V5_2_{}".format(i) for i in range(1, 11)],
    *["V6_2_{}".format(i) for i in range(1, 11)],
    "Class",
]
df = pd.read_csv(IN_FILE, names=names)

if __name__ == "__main__":
    pd.set_option("display.max_columns", 10)
    pd.set_option("display.max_rows", 5)
    for column in df:
        column_values = df.loc[:, (column,)]
        unknowns = column_values[column_values == "?"].count().sum()
        minimum = column_values.min().iloc[0]
        maximum = column_values.max().iloc[0]
        if unknowns > 10:
            print("Column {} has more than 10 unknowns, dropping it".format(column))
            df.drop(column, inplace=True, axis=1)
        elif minimum == maximum:
            print("Column {} has only identical values, dropping it".format(column))
            df.drop(column, inplace=True, axis=1)
        elif unknowns > 0:
            numeric_values = pd.to_numeric(
                column_values[column_values[column].apply(lambda x: x.isnumeric())][
                    column
                ]
            )
            avg = numeric_values.sum() / numeric_values.count()
            print(
                "Column {} has less than 10 unknowns, replacing them with {}".format(
                    column, avg
                )
            )
            df[column] = df[column].replace(["?"], avg)
    # Normalize the DF
    df = df.apply(pd.to_numeric)
    normalized_df = (df - df.min()) / (df.max() - df.min())
    # Except the class column
    normalized_df["Class"] = df["Class"] - 1
    normalized_df.to_csv(OUT_FILE, index=False)
