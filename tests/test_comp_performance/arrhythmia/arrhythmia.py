import kalypso
import keras
import numpy as np

# load the pretrained model
m = keras.models.load_model("arrhythmia.h5")
# extract the model to the weights folder
kalypso.export_weights(m, "weights")
