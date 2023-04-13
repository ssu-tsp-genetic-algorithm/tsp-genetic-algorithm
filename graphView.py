import pandas as pd
import matplotlib.pyplot as plt

# csv 파일 읽어오기
df = pd.read_csv("./output/bestRoute.csv", header=None)

# 경로의 total cost
cost = df.iloc[0,0]

# 좌표 그래프 생성
fig, ax = plt.subplots()
ax.set_title(f'Total cost : {cost}');

# 경로 연결선 그리기
for i in range(1, len(df)):
    row = df.iloc[i]
    ax.plot([row[1], row[3]], [row[0], row[2]], 'ro-')

# 그래프 보여주기
plt.show()
