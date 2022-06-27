import kalypso
import keras
import sys

if len(sys.argv) == 1:
    print("Usage: {} file.h5".format(sys.argv[0]))
    exit(1)

# load the pretrained model
m = keras.models.load_model(sys.argv[1])
m.summary()
# extract the model to the weights folder
kalypso.export_weights(m, "weights")
