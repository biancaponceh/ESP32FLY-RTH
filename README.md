# 🚀 ESP32FLY - Return to Home (RTH) System

## Código para sistema de retorno automático de cohetes modelo

### 📋 Descripción
Sistema que detecta la eyección del paracaídas y navega automáticamente al punto de lanzamiento usando ESP32 y GPS.

### 🔧 Características
- Detección de eyección con delay de seguridad
- Navegación RTH (Return to Home)
- Control de paracaídas/alerones
- Telemetría en tiempo real
- Simulación integrada

### ⚡ Hardware
- ESP32
- Módulo GPS
- Servo para paracaídas
- Buzzer y LED de estado

### 🎮 Uso
```cpp
// El sistema opera en 5 modos automáticos:
// 1. DORMANT - Espera lanzamiento
// 2. EYECCIÓN - Detecta separación  
// 3. RTH_NAVIGATE - Navega a home
// 4. RTH_DESCEND - Controla descenso
// 5. LANDED - Misión completada
