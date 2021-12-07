import pandas as pd
import numpy as np
import prose.datainsights as di
import os

sensors = ["acc", "Gyroscope"]
activities = ["lying", "running", "sitting", "standing", "walking"] 
positions = ["head", "shin", "thigh", "upperarm", "waist", "chest"] 
all_persons = ['person' + str(i) for i in range(1, 16)]

data_source = "data/uncompressed/har/"

def get_df(source):
    df = pd.read_csv(source, index_col=0)
    return df

df_dict = dict()
for person in all_persons:      
    if person not in df_dict:
        df_dict[person] = dict()
        
    cur_person_data = data_source + person + "/"
    for activity in activities:        
        df = None
        for position in positions:        
            for sensor in sensors:                
                for file in os.listdir(cur_person_data):                   
                    if not file.startswith("_"): continue
                    if activity in file and sensor in file and position in file:                        
                        if df is None:
                            df = get_df(cur_person_data + file)
                        else:
                            df = pd.concat([df, get_df(cur_person_data + file)], axis=1, ignore_index=True)
        df_dict[person][activity] = df
violation_matrix = pd.DataFrame(0.0,
columns = all_persons, index=all_persons)

for person1 in all_persons:
    # print("Learning assertion on", person1)     
    valid = 0
    for activity in activities:
        train_df = df_dict[person1][activity]
        assertions = di.learn_assertions(train_df[:5000], max_col_in_slice = 80, max_self_violation = 1)
        # print(activity, assertions.size(), assertions._impl.get_inv_count())
        if assertions.size() == 0: continue
            
        # print("Testing on others ... ")
        valid += 1
        for person2 in all_persons:
            #print(person2, end = ", ")     
            test_df = df_dict[person2][activity]
            violation = assertions.evaluate(test_df[5000:]).avg_violation
            violation_matrix.at[person1, person2] += violation    
        # print('')
        
    violation_matrix.loc[person1] = violation_matrix.loc[person1]/max(1, valid)
    
F = "Female"
m = "Male"

U = "Underweight"
N = "Normal"
Ov ="Overweight"
O = "Obese"

H = "High"
L = "Low"
M = "Moderate"

pd.set_option('display.precision', 1)
violation_matrix_temp = violation_matrix.apply(lambda x: round(x, 1))
violation_matrix_temp = pd.DataFrame(np.array(violation_matrix_temp), columns=["p"+str(i) for i in range(1, 16)], index=["p"+str(i) for i in range(1, 16)])

violation_matrix_temp["Fitness"] = [M, M, M, M, M, H, M, L, H, M, M, M, M, H, L]
violation_matrix_temp["BMI"] = [U, N, Ov, N, N, N, Ov, O, Ov, O, N, N, N, N, N]
violation_matrix_temp["Gender"] = [F,m,m,m,m,F,m,F,m,m,F,F,F,m,F]

pretty = violation_matrix_temp.style.background_gradient(cmap='Blues', subset=["p"+str(i) for i in range(1, 16)]) # with max_self_violation 1

tmp = os.path.join("Plots", "Figure_7.html")
with open(tmp, 'w') as f:
    f.write(pretty._repr_html_().replace('><thead> ', 'style="border-spacing: 0;"><thead> '))