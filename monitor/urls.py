from django.urls import path
from . import views

urlpatterns = [
    path('', views.dashboard, name='dashboard'),
    path('api/data/', views.receive_data, name='receive_data'),
    path('api/latest/', views.get_latest, name='get_latest'),
    path('api/history/', views.get_history, name='get_history'),
]
