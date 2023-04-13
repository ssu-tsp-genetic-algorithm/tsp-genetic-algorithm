import pandas as pd
import matplotlib.pyplot as plt

# csv 파일 읽어들이기
df = pd.read_csv('./output/fitnessChange.csv')

# 증감 추이 그래프 그리기
plt.plot(df)
plt.xlabel('Index')
plt.ylabel('Value')
plt.title('Trend of Values')
plt.show()
