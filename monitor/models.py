from django.db import models


class SensorReading(models.Model):
    device_id = models.CharField(max_length=50)
    spo2 = models.FloatField()
    heart_rate = models.FloatField()
    valve_status = models.BooleanField(default=False)
    battery = models.FloatField()
    timestamp = models.DateTimeField(auto_now_add=True)

    class Meta:
        ordering = ['-timestamp']

    def __str__(self):
        return f"{self.device_id} | {self.spo2}% | {self.timestamp:%H:%M:%S}"
