#include <Arduino.h>

// Page HTML pour la configuration
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Configuration Balise Statique</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <style>
    body { font-family: Arial, sans-serif; margin: 20px; background: #1a1a2e; color: #eee; }
    .container { max-width: 500px; margin: 0 auto; }
    h1 { color: #00d4ff; text-align: center; }
    h2 { color: #00d4ff; border-bottom: 1px solid #00d4ff; padding-bottom: 10px; }
    .card { background: #16213e; padding: 20px; border-radius: 10px; margin-bottom: 20px; }
    label { display: block; margin: 10px 0 5px; color: #aaa; }
    input[type="number"], input[type="text"] { 
      width: 100%; padding: 10px; border: 1px solid #0f3460; 
      border-radius: 5px; background: #0f3460; color: #fff; box-sizing: border-box;
    }
    button { 
      background: #00d4ff; color: #000; padding: 12px 24px; 
      border: none; border-radius: 5px; cursor: pointer; margin: 5px; font-weight: bold;
    }
    button:hover { background: #00a8cc; }
    button.danger { background: #e94560; color: #fff; }
    button.danger:hover { background: #c73e54; }
    .status { padding: 15px; border-radius: 5px; margin: 10px 0; }
    .success { background: #1b4332; color: #95d5b2; }
    .info { background: #0f3460; color: #00d4ff; }
    .value { font-size: 24px; font-weight: bold; color: #00d4ff; }
    .calibrating { animation: pulse 1s infinite; }
    @keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.5; } }
  </style>
</head>
<body>
  <div class="container">
    <h1>&#128225; Balise Statique</h1>
    <div class="card">
      <h2>&#128246; Informations</h2>
      <p>ID: <strong>%ID%</strong></p>
      <p>IP: <strong>%IP%</strong></p>
      <p>MAC: <strong>%MAC%</strong></p>
    </div>
    
    <div class="card">
      <h2>&#127919; Calibration RSSI à 1m</h2>
      <p>Placez l'appareil mobile à exactement 1 mètre de la balise, puis lancez la calibration.</p>
      <p>RSSI actuel à 1m: <span class="value" id="rssi1m">%RSSI1M%</span> dBm</p>
      <div id="calibStatus" class="status info" style="display:none;"></div>
      <button onclick="startCalibration()">&#9654; Démarrer calibration (10s)</button>
      <br><br>
      <label>Ou définir manuellement:</label>
      <input type="number" id="manualRssi" value="%RSSI1M%" step="0.1">
      <button onclick="setManualRssi()">Appliquer</button>
    </div>
    
    <div class="card">
      <h2>&#128200; Filtre de Kalman</h2>
      <p>Ajustez les paramètres du filtre pour optimiser le lissage du RSSI.</p>
      <label>Q (Bruit de processus) - Plus élevé = plus réactif:</label>
      <input type="number" id="kalmanQ" value="%Q%" step="0.01" min="0.001" max="10">
      <label>R (Bruit de mesure) - Plus élevé = plus lisse:</label>
      <input type="number" id="kalmanR" value="%R%" step="0.1" min="0.1" max="100">
      <br><br>
      <button onclick="setKalman()">&#128190; Sauvegarder</button>
      <button class="danger" onclick="resetKalman()">&#128260; Réinitialiser filtre</button>
    </div>
  </div>

  <script>
    function startCalibration() {
      document.getElementById('calibStatus').style.display = 'block';
      document.getElementById('calibStatus').innerHTML = '<span class="calibrating">Calibration en cours... Patientez 10 secondes</span>';
      fetch('/calibrate').then(r => r.text()).then(d => {
        setTimeout(checkCalibration, 11000);
      });
    }
    
    function checkCalibration() {
      fetch('/calibration_result').then(r => r.json()).then(d => {
        document.getElementById('rssi1m').innerText = d.rssi.toFixed(1);
        document.getElementById('manualRssi').value = d.rssi.toFixed(1);
        document.getElementById('calibStatus').className = 'status success';
        document.getElementById('calibStatus').innerHTML = '&#10004; Calibration terminée! Nouveau RSSI: ' + d.rssi.toFixed(1) + ' dBm (basé sur ' + d.count + ' mesures)';
      });
    }
    
    function setManualRssi() {
      const val = document.getElementById('manualRssi').value;
      fetch('/set_rssi?value=' + val).then(r => r.text()).then(d => {
        document.getElementById('rssi1m').innerText = val;
        alert('RSSI à 1m mis à jour!');
      });
    }
    
    function setKalman() {
      const q = document.getElementById('kalmanQ').value;
      const r = document.getElementById('kalmanR').value;
      fetch('/set_kalman?q=' + q + '&r=' + r).then(res => res.text()).then(d => {
        alert('Paramètres Kalman sauvegardés!');
      });
    }
    
    function resetKalman() {
      fetch('/reset_kalman').then(r => r.text()).then(d => {
        alert('Filtre Kalman réinitialisé!');
      });
    }
  </script>
</body>
</html>
)rawliteral";