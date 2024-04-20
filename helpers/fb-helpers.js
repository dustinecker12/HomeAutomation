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

export function writeAcUnitParams(
  targetTemp,
  actualTemp,
  onOffButton,
  overrideEnabled
) {
  set(ref(db, 'acParams/'), {
    targetTemp: targetTemp,
    actualTemp: actualTemp,
    onOffButton: onOffButton,
    overrideEnabled: overrideEnabled,
  });
}

export function setupAcUnitParamsListener(updateFunc) {
  onValue(ref(db, 'acParams/'), (snapshot) => {
    if (snapshot.val()) {
      updateFunc(snapshot.val());
    }
  });
}
