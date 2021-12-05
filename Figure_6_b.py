import prose.datainsights as di
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import subprocess
from sklearn.decomposition import PCA
import numpy as np
import numpy.random as rnd
import matplotlib.pyplot as plt
import os
import sys
from IPython.display import HTML
import warnings
from matplotlib import rc
from matplotlib import rcParams
import copy
np.random.seed(0)

rcParams['font.family'] = 'sans'
rc('text', usetex=True)

warnings.filterwarnings('ignore')

df_dict = None

data_source = "data/uncompressed/har/"

sensors = ["acc", "Gyroscope"]
activities = ["lying", "running", "sitting", "standing", "walking"]
positions = ["head", "shin", "thigh", "upperarm", "waist", "chest"]
persons = ['person' + str(i) for i in range(1, 16)]


def generate_df_dict():
    global df_dict
    df_dict = dict()

    for person in persons:
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
                            cur_df = pd.read_csv(cur_person_data + file, index_col=0)
                            if df is None:
                                df = cur_df
                            else:
                                df = pd.concat([df, cur_df], axis=1, ignore_index=True)
            df_dict[person][activity] = pd.DataFrame(df, dtype=float)
generate_df_dict()

def get_data_for_activities(cur_activities, N=100):
    if df_dict is None:
        print("Re-computing df-dict")
        generate_df_dict()

    train_df = None
    for activity in cur_activities:
        for person in persons[4:]:
            cur_df = copy.deepcopy(df_dict[person][activity])
            cur_df["person"] = person
            if train_df is None:
                train_df = cur_df
            else:
                train_df = pd.concat([train_df, cur_df], axis=0)
            X = np.array(np.array(train_df)[:,:-1], dtype=float)

    X = np.array(np.array(train_df)[:,:-1], dtype=float)
    y = np.array(train_df)[:,-1]

    sampled_indexes = np.random.choice(range(X.shape[0]), N)
    return pd.DataFrame(X[sampled_indexes], dtype=float), X[sampled_indexes], y[sampled_indexes]


sedentary_activities = ['lying', 'sitting', 'standing']
mobile_activities = ['running', 'walking']

import pandas as pd
import numpy as np
import prose.datainsights as di
import matplotlib.pyplot as plt
from sklearn.linear_model import LogisticRegression
from scipy.stats.stats import pearsonr

N = 5000  # number of data points in the train/test set
n_test_cases = 10  # number of experiments to run
step = 5

np.random.seed(1)

score_drops = []
our_violations = []

kk = 10
for kk in range(0, 51, step):
    mix_fraction = (kk * N) // 100
    print("Fraction of mobile data: ", kk, '%')

    score_drop_ = 0
    our_violation_ = 0
    cs = 0

    train_df, train_X, train_y = get_data_for_activities(mobile_activities, N=N)

    for _ in range(n_test_cases):
        test_df_1, test_X_1, test_y_1 = get_data_for_activities(mobile_activities, N=mix_fraction)
        test_df_2, test_X_2, test_y_2 = get_data_for_activities(sedentary_activities, N=N - mix_fraction)
        test_df = pd.concat([test_df_1, test_df_2], axis=0)
        test_X = np.vstack((test_X_1, test_X_2))
        test_y = np.concatenate([test_y_1, test_y_2], axis=0)

        assertions = di.learn_assertions(pd.DataFrame(test_X), max_self_violation=0.85)
        clf = LogisticRegression(random_state=0, solver='lbfgs', multi_class='multinomial')
        clf.fit(test_X, test_y)

        max_score = clf.score(test_X, test_y)

        score = clf.score(train_X, train_y)
        score_drop = max_score - score
        our_violation = assertions.evaluate(pd.DataFrame(train_X)).avg_violation

        score_drop_ += score_drop
        our_violation_ += our_violation
        cs += 1

        # print(cs, end='')

    # print('')

    score_drop = score_drop_ / cs
    our_violation = our_violation_ / cs

    # print("Drift fraction:", mix_fraction / float(N))
    # print("ML score drop:", score_drop)
    # print("Violations Our:", our_violation)

    score_drops.append(score_drop)
    our_violations.append(our_violation)

    # if len(score_drops) > 1 and cs % 10 == 0:
        # print("Done", cs, "tests")
        # print("PCC and P-Value", pearsonr(score_drops, our_violations))

systemName = 'CCSynth'

# print(score_drops)
# print(our_violations)

print("PCC & P-Value", pearsonr(score_drops,our_violations))

# our_violations = [0.38666818562854066, 0.1265843622117037, 0.06980683859973677, 0.04284904880355313, 0.02795662730789662, 0.02108232003713504, 0.015215993190774876, 0.011677844742624718, 0.008839468644942597, 0.0063424241999867955, 0.005461418127516672]
# score_drops =    [0.7059799999999999,  0.5723,             0.50674,             0.45854,             0.4145199999999999,  0.35785999999999996, 0.3154,               0.27490000000000003,  0.24375999999999998,  0.18882,               0.15572]

fig = plt.gcf()
fig.set_size_inches(3, 3)
rcParams['font.family'] = 'serif'
rc('text', usetex=True)
rcParams['figure.dpi'] = 300

plt.plot(np.arange(1, 12, 1) * 5.0, our_violations, "C3s", label='CCSynth')
plt.plot(np.arange(1, 12, 1) * 5.0, score_drops, "C0o", label="Classifier (LR)")

plt.legend(ncol=1, loc='upper right', handletextpad=0.1, handlelength=1)
plt.ylim([0,1])
plt.xlabel("Noise (\%) during training")
plt.ylabel("CC Violation/acc-drop", fontsize=14)
plt.xticks(np.arange(1, 12, 2) * 5)

plt.savefig(os.path.join("Plots", "Figure_6_b.pdf"), bbox_inches="tight")