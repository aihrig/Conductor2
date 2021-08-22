import { Injectable } from '@angular/core';
import { Observable, of } from 'rxjs';
import { Telemetry } from '../interfaces/telemetry';

@Injectable({
    providedIn: 'root',
})
export class TelemetryService {
    constructor() {}

    telemetryData!: Telemetry;
    getSpeed(): Observable<any> {
      return of(68);
    }

}
