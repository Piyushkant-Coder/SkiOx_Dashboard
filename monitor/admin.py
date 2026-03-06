from django.contrib import admin
from .models import SensorReading

@admin.register(SensorReading)
class SensorReadingAdmin(admin.ModelAdmin):
    list_display = ('device_id', 'spo2', 'heart_rate', 'valve_status', 'battery', 'timestamp')
    list_filter = ('device_id', 'valve_status')
    ordering = ('-timestamp',)
