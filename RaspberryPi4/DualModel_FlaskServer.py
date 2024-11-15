# Author: Imaar Rathur
# Date: 13/10/2024
# I/O: Sensor Data, Model Output
# Source: https://flask.palletsprojects.com/en/3.0.x/, https://scikit-learn.org/stable/supervised_learning.html with some assistance from ChatGPT

from flask import Flask, request, jsonify
import joblib
import numpy as np

app = Flask(__name__)

# Load the trained models and scaler
model1 = joblib.load('model1.pkl')
model2 = joblib.load('model2.pkl')
scaler = joblib.load('scaler.pkl')

@app.route('/sensor', methods=['POST'])
def receive_data():
    try:
        # Parse the incoming JSON data
        data = request.get_json()

        # Extract only 'current_mA' and 'temperature' for scaling and prediction
        current_mA = data['current_mA']
        temperature = data['temperature']

        # Create the input array (only current and temperature)
        input_data = np.array([[current_mA, temperature]])

        # Scale the input data
        input_data_scaled = scaler.transform(input_data)

        # Make predictions for both transistors
        transistor1_state = model1.predict(input_data_scaled)[0]
        transistor2_state = model2.predict(input_data_scaled)[0]

        # Prepare the response
        response = {
            'transistor1_state': int(transistor1_state),
            'transistor2_state': int(transistor2_state)
        }

        # Send back the transistor states as a JSON response
        return jsonify(response)

    except Exception as e:
        print(f"Error: {e}")
        return "Error occurred", 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
