const int dry = 860; //dry value
const int wet = 410; //wet value


int get_soil_moisture_value() {
  int sensorVal = analogRead(A0);
  int percentageHunidity = map(sensorVal, wet, dry, 100, 0);
  
  return percentageHunidity;
}
