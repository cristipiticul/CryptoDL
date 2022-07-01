import medmnist
import numpy as np
import keras
import tensorflow as tf
from keras import layers

DATASET_FOLDER = "../../datasets/chestmnist/"
OUT_FOLDER = "../../tests/test_comp_performance/chest/"
OUT_FILE_TEMPLATE = "chestmnist_K{}.h5"
PREDICTIONS_FILE_TEMPLATE = "prediction_K{}.csv"

input_shape = (28, 28, 1)
NUM_FILTERS = 2


def get_first_layer_output(model, data):
    # create new model
    new_model = keras.Sequential(
        [
            layers.Conv2D(
                NUM_FILTERS,
                kernel_size=(5, 5),
                strides=(2, 2),
                activation=tf.math.square,
                name="conv2d_1",
                padding="valid",
            ),
            layers.Flatten(),
        ]
    )
    new_model.build(input_shape=(1, *input_shape))

    # set weights of the first layer
    new_model.set_weights(model.layers[0].get_weights())

    # compile it after setting the weights
    new_model.compile(optimizer="adam", loss="categorical_crossentropy")

    # get output of the first dens layer
    return new_model.predict(data)


# inspired from https://keras.io/examples/vision/mnist_convnet/
def main():
    medmnist.ChestMNIST(split="train", root=DATASET_FOLDER, download=True)
    npz_file = np.load(DATASET_FOLDER + "chestmnist.npz")
    # normalize
    x_train = npz_file["train_images"] / 255
    y_train = npz_file["train_labels"]
    # use only the first 16384 examples

    num_classes = 14
    # Add the extra dim
    x_train = np.expand_dims(x_train, -1)

    # model = tf.keras.models.load_model(
    #     OUT_FOLDER + "chestmnist.h5", custom_objects={"square": tf.math.square}
    # )

    model = keras.Sequential(
        [
            keras.Input(shape=input_shape),
            layers.Conv2D(
                NUM_FILTERS,
                kernel_size=(5, 5),
                strides=(2, 2),
                activation=tf.math.square,
                name="conv2d_1",
                padding="valid",
            ),
            # layers.Conv2D(
            #     32,
            #     kernel_size=(5, 5),
            #     strides=(2, 2),
            #     activation=tf.math.square,
            #     name="conv2d_2",
            #     padding="valid",
            # ),
            layers.Flatten(name="flatten"),
            # layers.Dense(32, activation=tf.math.square, name="dense_1"),
            layers.Dense(num_classes, name="dense_2"),
        ]
    )
    batch_size = 128
    epochs = 1
    model.compile(
        # it is a multi-label binary-class classification task
        loss=tf.keras.losses.BinaryCrossentropy(),
        optimizer=tf.keras.optimizers.Adam(learning_rate=1e-3),
        metrics=["accuracy"],
    )
    model.fit(
        x_train, y_train, batch_size=batch_size, epochs=epochs, validation_split=0.1
    )
    model.summary()
    model.save(OUT_FOLDER + OUT_FILE_TEMPLATE.format(NUM_FILTERS))

    ckks_slots = 16384
    x_train = x_train[:ckks_slots, :]
    # y_train = y_train[:ckks_slots, :]
    predicted = model(x_train).numpy()
    # print(get_first_layer_output(model, x_train[0:1, :, :]))
    # print(model(x_train[0:1, :, :, :]))
    np.savetxt(
        OUT_FOLDER + PREDICTIONS_FILE_TEMPLATE.format(NUM_FILTERS),
        predicted,
        fmt="%0.20f",
        delimiter=",",
    )


if __name__ == "__main__":
    main()
