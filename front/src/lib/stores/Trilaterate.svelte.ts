import { SensorStore } from "./Sensors.svelte";

class Trilaterate {
    ROOM_SIZE =        $state(6);    // 6x6 mètres
    N_ENV =            $state(2.5);  // Facteur d'atténuation (2.0 à 4.0, ajustable)
    SMOOTHING_FACTOR = $state(0.3);  // Facteur de lissage exponentiel (0.1 = très lisse, 0.5 = réactif)

    readings =         $state<Record<string, number>>({ 'esp_1': -100, 'esp_2': -100, 'esp_3': -100 });

    smoothedPosition = $derived({ x: this.ROOM_SIZE / 2, y: this.ROOM_SIZE / 2 });

    sensorStore = new SensorStore()

    trilaterateStrategy = $state<'weighted' | 'simple'>('weighted');

    rawPosition = $derived.by(() => {
        const beaconData: Array<{ x: number; y: number; distance: number }> = [];

        for (const [key, sensor] of Object.entries(this.sensorStore.sensors)) {
            const rssi = this.readings[key];
            const dist = this.getDistance(rssi, sensor.rssi1m);
            beaconData.push({ x: sensor.x, y: sensor.y, distance: dist });
        }

        const pos = this.trilaterate(beaconData);
        
        const margin = 1;
        const x = Math.max(-margin, Math.min(this.ROOM_SIZE + margin, pos.x));
        const y = Math.max(-margin, Math.min(this.ROOM_SIZE + margin, pos.y));
        return { x, y };
    });

    position = $derived.by(() => {
        const raw = this.rawPosition;
        this.smoothedPosition = {
            x: this.smoothedPosition.x + this.SMOOTHING_FACTOR * (raw.x - this.smoothedPosition.x),
            y: this.smoothedPosition.y + this.SMOOTHING_FACTOR * (raw.y - this.smoothedPosition.y)
        };

        return this.smoothedPosition;
    });

    getDistance(rssi: number, txPower: number): number {
        const clampedRSSI = Math.max(-100, Math.min(-30, rssi));
        return Math.pow(10, (txPower - clampedRSSI) / (10 * this.N_ENV));
    }

    trilaterate(beacons: Array<{ x: number; y: number; distance: number }>): { x: number; y: number } {
        if (this.trilaterateStrategy === 'weighted') {
            return this.trilaterate_weighted(beacons);
        } else {
            return this.trilaterate_simple(beacons);
        }
    }

    // --- TRILATÉRATION SIMPLE (barycentre pondéré par l'inverse du carré de la distance) ---
    trilaterate_weighted(beacons: Array<{ x: number; y: number; distance: number }>): { x: number; y: number } {
        if (beacons.length === 0) {
            return { x: this.ROOM_SIZE / 2, y: this.ROOM_SIZE / 2 };
        }

        let xNum = 0;
        let yNum = 0;
        let weightSum = 0;

        for (const beacon of beacons) {
            // Clamp pour éviter les divisions par zéro ou distances absurdes
            const d = Math.max(0.3, Math.min(20, beacon.distance));
            const w = 1 / (d * d + 0.0001);
            xNum += beacon.x * w;
            yNum += beacon.y * w;
            weightSum += w;
        }

        return { x: xNum / weightSum, y: yNum / weightSum };
    }

    trilaterate_simple(beacons: Array<{ x: number; y: number; distance: number }>): { x: number; y: number } {
        if (beacons.length === 0) {
            return { x: this.ROOM_SIZE / 2, y: this.ROOM_SIZE / 2 };
        }

        // Moyenne arithmétique simple des positions
        const avgX = beacons.reduce((sum, b) => sum + b.x, 0) / beacons.length;
        const avgY = beacons.reduce((sum, b) => sum + b.y, 0) / beacons.length;

        return { x: avgX, y: avgY };
    }

    isInDangerZone(dangerArea: { x: number; y: number; width: number; height: number }): boolean {
        const pos = this.position;
        return (
            pos.x >= dangerArea.x &&
            pos.x <= dangerArea.x + dangerArea.width &&
            pos.y >= dangerArea.y &&
            pos.y <= dangerArea.y + dangerArea.height
        );
    }
}

export const tril = new Trilaterate();