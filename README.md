#學校網管系統

#指導老師：朱彥銘

#組員：莊博馨、曾瓊瑩、蔡心敏、林薏庭、陳沛穎

內容:從Client端Ping各段閘道網路，校外網路用DNS測試，定時將ping的值回傳到Splunk,再由Splunk分析，若回傳時間超過訂時間，便發出Alert提醒網管人員

效益:讓網管人員即時知道哪段網路出問題

HOW:用PYTHON寫好的程式，使用MQTT傳輸協定傳送至SPLUNK，在使用SPLUNK告警功能發出錯誤的LOG檔

甘特圖設計https://www.smartapp.com/gantterforgoogledrive/index.html?fileID=0B2NixI49LfkmSXF6SVlWZFc2Rms#

##功能性需求

1.偵測是否斷線

2.監控網段連線狀態

3.有異常的網段發出告警

4.分析各時段的連線品質

##非功能性需求

1.有異常即時給使用者知道

![](01.PNG)

