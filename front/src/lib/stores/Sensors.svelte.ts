import type { Sensor } from "../../types";

const defaultSensors: Record<string, Sensor> = {
    'esp_1': { x: 2, y: 1, color: 'bg-red-500',   rssi1m: -59 },
    'esp_2': { x: 4, y: 1, color: 'bg-green-500', rssi1m: -59 },
    'esp_3': { x: 4, y: 4, color: 'bg-blue-500',  rssi1m: -59 }
};

export class SensorStore {
    sensors = $state<Record<string, Sensor>>(structuredClone(defaultSensors));

    resetCalibration() {
        this.sensors = structuredClone(defaultSensors);
    }
}