import pandas as pd
import numpy as np
from sklearn import preprocessing
import prose.datainsights as di
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_absolute_error
import os

np.random.seed(0)
le = preprocessing.LabelEncoder()

columns = ["Year","Month","Day","DayOfWeek","CRSDepartureTime","CRSArrivalTime","UniqueCarrier","FlightNumber","ActualElapsedTime","Origin","Destination","Distance","Diverted","ArrivalDelay"]
df = pd.read_csv(os.path.join("data", "uncompressed", "2008_14col.data"), header=None, names=columns)
df = df[["Month","Day","CRSDepartureTime","CRSArrivalTime","UniqueCarrier","FlightNumber","ActualElapsedTime","Origin","Destination","Distance","Diverted","ArrivalDelay"]]
for col in ["UniqueCarrier",  "FlightNumber", "Origin", "Destination"]:    
    df[col] = le.fit_transform(df[col])
    
sameDay = df[df.CRSArrivalTime > df.CRSDepartureTime]  # This happens when the arrival is on same day
diffDay = df[df.CRSArrivalTime <= df.CRSDepartureTime] # This happens when the arrival is on next day

y = np.array(sameDay.ArrivalDelay)
x = np.array(sameDay.drop(columns=["ArrivalDelay"]))
x_train, x_test, y_train, y_test = train_test_split(x, y, train_size=0.95, test_size=0.05, random_state=0)

y_test_drifted = np.array(diffDay.ArrivalDelay)
x_test_drifted = np.array(diffDay.drop(columns=["ArrivalDelay"]))
x_test_all = np.vstack((x_test, x_test_drifted))
y_test_all = np.hstack((y_test, y_test_drifted))

x_train_df = pd.DataFrame(x_train, columns = [  "Month", "Day", "CRSDepartureTime", "CRSArrivalTime", "UniqueCarrier", "FlightNumber", "ActualElapsedTime", "Origin", "Destination", "Distance", "Diverted"])
x_test_df = pd.DataFrame(x_test, columns = [  "Month", "Day", "CRSDepartureTime", "CRSArrivalTime", "UniqueCarrier", "FlightNumber", "ActualElapsedTime", "Origin", "Destination", "Distance", "Diverted"])
x_test_drifted_df = pd.DataFrame(x_test_drifted, columns = [  "Month", "Day", "CRSDepartureTime", "CRSArrivalTime", "UniqueCarrier", "FlightNumber", "ActualElapsedTime", "Origin", "Destination", "Distance", "Diverted"])
x_test_all_df = pd.DataFrame(x_test_all, columns = [  "Month", "Day", "CRSDepartureTime", "CRSArrivalTime", "UniqueCarrier", "FlightNumber", "ActualElapsedTime", "Origin", "Destination", "Distance", "Diverted"])

assertions = di.learn_assertions(x_train_df, max_self_violation=1)

v_train = assertions.evaluate(x_train_df).avg_violation
v_daytime = assertions.evaluate(x_test_df).avg_violation
v_overnight = assertions.evaluate(x_test_drifted_df).avg_violation
v_mixed = assertions.evaluate(x_test_all_df).avg_violation

reg = LinearRegression().fit(x_train, y_train)

mae_train = mean_absolute_error(y_train, reg.predict(x_train))
mae_daytime = mean_absolute_error(y_test, reg.predict(x_test))
mae_overnight = mean_absolute_error(y_test_drifted, reg.predict(x_test_drifted))
mae_mixed = mean_absolute_error(y_test_all, reg.predict(x_test_all))

print("------------------------------------------------------------------------")
print("                                          Serving")
print("                             -------------------------------------------")
print("                     Train   |      Daytime   |   Overnight  |  Mixed")
print("------------------------------------------------------------------------")
print("Average Violation", end = ' |')
print('{:6.2f}'.format(v_train * 100) + '%', end = '   |')
print('{:12.2f}'.format(v_daytime * 100) + '%', end = '   |')
print('{:8.2f}'.format(v_overnight * 100) + '%', end = '     |')
print('{:6.2f}'.format(v_mixed * 100) + '%', end = '\n')
print("       MAE       ", end = ' |')
print('{:7.2f}'.format(mae_train), end = '   |')
print('{:13.2f}'.format(mae_daytime), end = '   |')
print('{:9.2f}'.format(mae_overnight), end = '     |')
print('{:7.2f}'.format(round(mae_mixed, 2)), end = '\n')
print("-------------------------------------------------------------------------")

fig_4 = open('Plots/Figure_4.txt', 'w')
fig_4.write("------------------------------------------------------------------------\n")
fig_4.write("                                          Serving\n")
fig_4.write("                             -------------------------------------------\n")
fig_4.write("                     Train   |      Daytime   |   Overnight  |  Mixed\n")
fig_4.write("------------------------------------------------------------------------\n")
fig_4.write("Average Violation |")
fig_4.write('{:6.2f}'.format(v_train * 100) + '%' + '   |')
fig_4.write('{:12.2f}'.format(v_daytime * 100) + '%' +  '   |')
fig_4.write('{:8.2f}'.format(v_overnight * 100) + '%' +  '     |')
fig_4.write('{:6.2f}'.format(v_mixed * 100) + '%' +  '\n')
fig_4.write("       MAE       " +  ' |')
fig_4.write('{:7.2f}'.format(mae_train) + '   |')
fig_4.write('{:13.2f}'.format(mae_daytime) +  '   |')
fig_4.write('{:9.2f}'.format(mae_overnight) +  '     |')
fig_4.write('{:7.2f}'.format(round(mae_mixed, 2)) +  '\n')
fig_4.write("-------------------------------------------------------------------------\n")
fig_4.close()