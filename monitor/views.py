from django.shortcuts import render
from rest_framework.decorators import api_view
from rest_framework.response import Response
from rest_framework import status

from .models import SensorReading
from .serializers import SensorReadingSerializer

CRITICAL_SPO2 = 90


def dashboard(request):
    latest = SensorReading.objects.first()
    history = SensorReading.objects.all()[:50]
    is_critical = latest and latest.spo2 < CRITICAL_SPO2
    
    return render(request, 'monitor/dashboard.html', {
        'latest': latest,
        'history': history,
        'is_critical': is_critical,
        'critical_threshold': CRITICAL_SPO2,
    })


@api_view(['POST'])
def receive_data(request):
    serializer = SensorReadingSerializer(data=request.data)
    if serializer.is_valid():
        serializer.save()
        return Response({'status': 'ok'}, status=status.HTTP_201_CREATED)
    return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)


@api_view(['GET'])
def get_latest(request):
    latest = SensorReading.objects.first()
    if not latest:
        return Response({'error': 'No data available'}, status=status.HTTP_404_NOT_FOUND)
    
    data = SensorReadingSerializer(latest).data
    data['is_critical'] = latest.spo2 < CRITICAL_SPO2
    return Response(data)


@api_view(['GET'])
def get_history(request):
    readings = SensorReading.objects.all()[:50]
    return Response(SensorReadingSerializer(readings, many=True).data)
