import tensorflow as tf
from tensorflow.keras.datasets import mnist
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Flatten, LeakyReLU
import numpy as np

# Load MNIST dataset
(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0  # Normalize pixel values

# Build a neural network model
model = Sequential([
    Flatten(input_shape=(28, 28)),  # Flatten the input to a 1D array (28*28 = 784)
    
    Dense(128),  # First hidden layer with 128 neurons
    LeakyReLU(alpha=0.1),  # Leaky ReLU activation

    Dense(64),  # Second hidden layer with 64 neurons
    LeakyReLU(alpha=0.1),  # Leaky ReLU activation
    
    Dense(32),  # Third hidden layer with 32 neurons
    LeakyReLU(alpha=0.1),  # Leaky ReLU activation
    
    Dense(10, activation='softmax')  # Output layer with 10 neurons (one per class)
])

# Compile the model
model.compile(optimizer='adam',
              loss='sparse_categorical_crossentropy',
              metrics=['accuracy'])

# Train the model
model.fit(x_train, y_train, epochs=10, validation_split=0.1)

# Evaluate the model on test data
test_loss, test_accuracy = model.evaluate(x_test, y_test)
print(f"Test accuracy: {test_accuracy}")

# Save weights and biases to a file
weights_and_biases = {}
for i, layer in enumerate(model.layers):
    if len(layer.get_weights()) > 0:
        weights, biases = layer.get_weights()
        weights_and_biases[f'layer_{i}_weights'] = weights
        weights_and_biases[f'layer_{i}_biases'] = biases

np.savez('model_weights_and_biases.npz', **weights_and_biases)
print("Model weights and biases saved to 'model_weights_and_biases.npz'")
