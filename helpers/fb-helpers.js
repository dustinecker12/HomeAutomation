import { getDatabase, onValue, ref, set } from 'firebase/database';

import { firebaseConfig } from './fb-credentials';
import { getApp, getApps, initializeApp } from 'firebase/app';

let app, db;

if (!getApps().length) {
  try {
    app = initializeApp(firebaseConfig);
    db = getDatabase(app);
  } catch (error) {
    console.error('Error initializing app: ' + error);
  }
} else {
  app = getApp();
  db = getDatabase(app);
}

export function writeAcUnitState(
  targetTemp,
  actualTemp,
  onOffButton,
  overrideEnabled
) {
  set(ref(db, 'acState/'), {
    targetTemp: targetTemp,
    actualTemp: actualTemp,
    onOffButton: onOffButton,
    overrideEnabled: overrideEnabled,
  });
}

export function setupAcUnitStateListener(updateFunc) {
  onValue(ref(db, 'acState/'), (snapshot) => {
    if (snapshot.val()) {
      updateFunc(snapshot.val());
    }
  });
}
