#!/usr/bin/python3
import numpy as np
import pandas as pd
import tensorflow as tf
import tensorflow.keras as keras

IN_FILE = "../../datasets/arrhythmia/arrhythmia_preproc.data"
OUT_FILE = "../../tests/test_comp_performance/arrhythmia/arrhythmia.h5"
NUM_TEST_ITEMS = 5

CLASS_MAX = 16


def to_one_hot(array):
    """
    Converts numpy array from [1, 2, 3] to [[1, 0, 0, ...], [0, 1, 0, ...], [0, 0, 1, ...]]"""
    shape = (array.size, CLASS_MAX)
    one_hot = np.zeros(shape)
    rows = np.arange(array.size)
    one_hot[rows, array] = 1
    return one_hot


if __name__ == "__main__":
    df = pd.read_csv(IN_FILE)
    df_train = df[:-NUM_TEST_ITEMS]
    df_test = df[-NUM_TEST_ITEMS:]
    df_train_features = df_train.copy()
    df_train_labels = df_train_features.pop("Class")
    df_test_features = df_test.copy()
    df_test_labels = df_test_features.pop("Class")

    train_features = np.array(df_train_features)
    train_labels = np.array(df_train_labels)
    test_features = np.array(df_test_features)
    test_labels = np.array(df_test_labels)

    train_labels_one_hot = to_one_hot(train_labels)
    test_labels_one_hot = to_one_hot(test_labels)

    train_features = tf.convert_to_tensor(train_features, dtype=tf.float32)
    test_features = tf.convert_to_tensor(test_features, dtype=tf.float32)
    train_labels_one_hot = tf.convert_to_tensor(train_labels_one_hot, dtype=tf.float32)
    test_labels_one_hot = tf.convert_to_tensor(test_labels_one_hot, dtype=tf.float32)

    model = keras.Sequential(
        [
            keras.layers.Dense(64, activation=tf.math.square, name="dense1"),
            keras.layers.Dense(16, name="dense2"),
            keras.layers.Softmax(),  # Softmax will be removed, but it's useful for training
        ]
    )

    model.compile(
        loss="categorical_crossentropy",
        optimizer=keras.optimizers.Adam(),
        metrics=["accuracy"],
    )

    model.fit(train_features, train_labels_one_hot, epochs=200)

    # model.summary()
    # Remove the Softmax layer -- it's not implemented in CryptoDL
    model.pop()
    # model.summary()

    model.save(OUT_FILE)
