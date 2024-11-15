# Author: Imaar Rathur
# Date: 13/10/2024
# I/O: Sensor Data, Model Output
# Source: https://flask.palletsprojects.com/en/3.0.x/, https://www.geeksforgeeks.org/writing-csv-files-in-python/

from flask import Flask, request, jsonify
import csv
import os

app = Flask(__name__)

# Path to save CSV file
csv_file = "sensor_data.csv"

# Check if the file already exists
if not os.path.exists(csv_file):
    # Create a CSV file and add the header
    with open(csv_file, mode='w') as file:
        writer = csv.writer(file)
        writer.writerow(['timestamp', 'voltage', 'current', 'temperature', 'humidity', 'transistor_state'])

@app.route('/sensor', methods=['POST'])
def receive_sensor_data():
    # Get the sensor data from Arduino (posted as JSON)
    data = request.get_json()

    # Extract sensor values
    voltage = data.get('bus_voltage')
    current = data.get('current_mA')
    temperature = data.get('temperature')
    humidity = data.get('humidity')
    # Optional: You can include labels or commands to train the model (e.g., transistor state)
    transistor_state = data.get('transistor_state', 0)  # Default to 0 if not provided

    # Get the current timestamp
    from datetime import datetime
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    # Write sensor data to CSV
    with open(csv_file, mode='a') as file:
        writer = csv.writer(file)
        writer.writerow([timestamp, voltage, current, temperature, humidity, transistor_state])

    # Send back a response
    return jsonify({'status': 'success', 'message': 'Data saved'}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
