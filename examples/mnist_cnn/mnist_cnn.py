import kalypso
import keras
import numpy as np

# load the pretrained model
m = keras.models.load_model("mnistCNN.h5")
for layer in m.layers:
    layer.set_weights(np.array(layer.get_weights()) + 0.00000001)
# extract the model to the weights folder
kalypso.export_weights(m, "weights")
