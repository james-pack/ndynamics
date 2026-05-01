#!/usr/bin/env python3

import numpy as np

# Define a matrix
A = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])

# Compute SVD
# U: Left singular vectors
# S: Singular values
# VT: Right singular vectors (transposed)
U, S, VT = np.linalg.svd(A)

# Convert the 1D array S into a 2D diagonal matrix
# We use the shapes of U and VT to ensure dimensions match
Sigma = np.zeros((U.shape[1], VT.shape[0]))
np.fill_diagonal(Sigma, S)

# Compute the dot product: U @ Sigma @ VT
A_reconstructed = U @ Sigma @ VT

print(f"A: {A}\nU: {U}\nS: {S}\nVT: {VT}\nA reconstructed: {A_reconstructed}")
