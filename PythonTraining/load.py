import numpy as np

def load_weights(file_path='model_weights_and_biases.npz'):
    # Load the weights and biases from the .npz file
    data = np.load(file_path)
    weights_and_biases = {}
    
    # Loop through the keys (which are the layer identifiers) and retrieve weights/biases
    for key in data:
        weights_and_biases[key] = data[key]
    
    return weights_and_biases

# Example usage
weights_and_biases = load_weights()
for key, value in weights_and_biases.items():
    print(f"{key}: shape {value.shape}")
