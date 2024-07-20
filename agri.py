import requests
import pandas as pd

API_KEY = "Q5R6WLKSRJAM64O8"
CHANNEL_ID = "2440023"

def get_thingspeak_data():
    url = f'https://api.thingspeak.com/channels/{channel_id}/feeds.json?api_key={api_key}&results=1'
    response = requests.get(url)
    data = response.json()
    
    if 'feeds' in data:
        return data['feeds'][0]
    else:
        return None

def predict_soil_type(moisture_value):
    if moisture_value < 30:
        return 'Dry'
    elif 30 <= moisture_value <= 60:
        return 'Humid'
    else:
        return 'Wet'

def predict_weather(temperature, humidity):
    if temperature > 25 and humidity > 70:
        return 'Rainy'
    elif temperature > 30:
        return 'Sunny'
    elif temperature > 20 and humidity < 40:
        return 'Windy'
    else:
        return 'Normal'

def get_water_requirement(crop_type, region, soil_type, weather):

    dataset=pd.read_csv(r'C:\Users\svvis\OneDrive\Desktop\typhon 91\DATASET.csv')
    print(dataset.head(10))
    dp=dataset.drop(['TEMPERATURE'],axis=1)
    print(dp.head(10))

    x=dp[['CROP TYPE','SOIL TYPE','REGION','WEATHER CONDITION']]
    x1=pd.get_dummies(x)
    print(x1)
    y=dp['WATER REQUIREMENT']
    print(x)
    print(y)
    print(dp.isnull().sum())

    from sklearn.model_selection import train_test_split
    xtrain, xtest, ytrain ,ytest=train_test_split(x1,y,test_size=0.2)
    print(xtrain.shape)
    print(xtest.shape)
    print(ytrain.shape)
    print(ytest.shape)

    from sklearn.tree import DecisionTreeRegressor #correct prediction
    model1=DecisionTreeRegressor()
    model1.fit(xtrain,ytrain)

    ypred=model1.predict(xtest)

    from sklearn.metrics import r2_score
    score=r2_score(ypred,ytest)
    s=1-score
    print(s)

    new_input = pd.DataFrame([[crop_type, soil_type, region, weather]], columns=['CROP TYPE', 'SOIL TYPE', 'REGION', 'WEATHER CONDITION'])
    new_input_encoded = pd.get_dummies(new_input)

    # Ensure new input has the same columns and order as x1
    missing_cols = set(x1.columns) - set(new_input_encoded.columns)
    for col in missing_cols:
        new_input_encoded[col] = 0

    new_input_encoded = new_input_encoded[x1.columns]

    # Make the prediction
    p = model1.predict(new_input_encoded)
    return float(p[0])

def updateLatestData(temp,humidity,moisture,water_req):
    reqUrl = f'https://api.thingspeak.com/update?api_key={API_KEY}&field1={temp}&field2={humidity}&field3={moisture}&field4={water_req}'

    print("Updating Data.....")

    response = requests.get(reqUrl)
    data = response.json()

    if type(data) is not int:
        print("Updating Failed")
    else:
        print("Updated")

    
   
# Get API keys and string inputs
api_key = 'Q5R6WLKSRJAM64O8'
channel_id = '2440023'
crop_type = input('Enter Crop Type: ')
region = input('Enter Region: ')

# Get real-time data from ThingSpeak
data = get_thingspeak_data()

# Extract values
temperature = float(data['field1'])
humidity = float(data['field2'])
moisture_value = float(data['field3'])
print(temperature)
print(humidity)
print(moisture_value)

# Predict soil type and weather
soil_type = predict_soil_type(moisture_value)
weather = predict_weather(temperature, humidity)
print(soil_type)
print(weather)

# Get water requirement
water_requirement = get_water_requirement(crop_type, region, soil_type, weather)

if water_requirement is not None:
    print(f'Water Requirement for {crop_type} in {region} under {soil_type} soil and {weather} weather: {water_requirement} mm')

    updateLatestData(temperature,humidity,moisture_value,water_requirement)
else:
    print('Data not available for the specified inputs.')
