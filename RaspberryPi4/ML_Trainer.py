# Author: Imaar Rathur
# Date: 13/10/2024
# I/O: Sensor Data, Model Output
# Source: https://flask.palletsprojects.com/en/3.0.x/, https://scikit-learn.org/stable/supervised_learning.html with some assistance from ChatGPT

import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score
import joblib

# Load the dataset
data = pd.read_csv('dataset.csv')

# Select only the relevant features: current and temperature
X = data[['current', 'temperature']]

# Target variables for the transistors
y1 = data['transistor1_state']  # Target for transistor 1
y2 = data['transistor2_state']  # Target for transistor 2

# Split the dataset into training and test sets for both transistors
X_train1, X_test1, y_train1, y_test1 = train_test_split(X, y1, test_size=0.2, random_state=42)
X_train2, X_test2, y_train2, y_test2 = train_test_split(X, y2, test_size=0.2, random_state=42)

# Standardize the features (current and temperature)
scaler = StandardScaler()
X_train1_scaled = scaler.fit_transform(X_train1)
X_test1_scaled = scaler.transform(X_test1)
X_train2_scaled = scaler.transform(X_train2)
X_test2_scaled = scaler.transform(X_test2)

# Train a logistic regression model for transistor 1
model1 = LogisticRegression()
model1.fit(X_train1_scaled, y_train1)

# Predict and evaluate model 1
y_pred1 = model1.predict(X_test1_scaled)
accuracy1 = accuracy_score(y_test1, y_pred1)
print(f"Model 1 (Transistor 1) Accuracy: {accuracy1}")

# Train a logistic regression model for transistor 2
model2 = LogisticRegression()
model2.fit(X_train2_scaled, y_train2)

# Predict and evaluate model 2
y_pred2 = model2.predict(X_test2_scaled)
accuracy2 = accuracy_score(y_test2, y_pred2)
print(f"Model 2 (Transistor 2) Accuracy: {accuracy2}")

# Save both models and the scaler
joblib.dump(model1, 'model1.pkl')
joblib.dump(model2, 'model2.pkl')
joblib.dump(scaler, 'scaler.pkl')
