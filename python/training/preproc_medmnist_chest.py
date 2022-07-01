import medmnist
import numpy as np

# from matplotlib import pyplot as plt
DATASET_FOLDER = "../../datasets/chestmnist/"

# inspired from https://github.com/MedMNIST/experiments/blob/main/MedMNIST2D/train_and_eval_autokeras.py
def main():
    medmnist.ChestMNIST(split="train", root=DATASET_FOLDER, download=True)
    npz_file = np.load(DATASET_FOLDER + "chestmnist.npz")
    ckks_slots = 16384
    # normalize
    x_train = npz_file["train_images"] / 255
    y_train = npz_file["train_labels"]

    # Show image
    # plt.imshow(x_train[10], interpolation="nearest")
    # plt.show()

    # write the first 16384 examples in a CSV
    x_train = x_train[:ckks_slots, :]
    y_train = y_train[:ckks_slots, :]
    print(x_train.shape, y_train.shape)
    x_train = x_train.reshape(ckks_slots, x_train.shape[1] * x_train.shape[2])
    print(x_train.shape, y_train.shape)
    full = np.concatenate((x_train, y_train), axis=1)
    print(full.shape)
    np.savetxt(DATASET_FOLDER + "chestmnist.csv", full, fmt="%0.20f", delimiter=",")


if __name__ == "__main__":
    main()
