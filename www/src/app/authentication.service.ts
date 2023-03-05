import { Injectable } from '@angular/core';
import {WebsocketService} from "./websocket.service";

@Injectable({
  providedIn: 'root'
})
export class AuthenticationService {
  username: string = "";
  password: string = "";
  authenticated: boolean = false;
  permissions: Array<string> = []

  constructor(private websocketService: WebsocketService) {
    websocketService.getEmitter().subscribe((event) => this.handleEvent(event));
    websocketService.getSocketEventEmitter().subscribe((event) => this.handleSocketEvent(event));
    const u = sessionStorage.getItem("user");
    const p = sessionStorage.getItem("password");
    if (!!u && !!p) {
      this.username = u;
      this.password = p;
      this.login(u, p);
    }
  }

  private handleSocketEvent(event: string) {
    if (this.authenticated && event == "open") {
      this.login(this.username, this.password);
    }
  }

  private handleEvent(event: any) {
    if (event['auth']) {
      if (event.auth.message == 'Authenticated Successfully') {
        sessionStorage.setItem("user", this.username);
        sessionStorage.setItem("password", this.password);
        // handle success
        if (event.auth.userdata.permissions) {
          this.permissions = event.auth.userdata.permissions;
        } else {
          this.permissions = [];
        }
        this.authenticated = true;
      }
      if (event.auth.message == 'Password Wrong') {
        // handle not successful
        this.authenticated = false;
      }
    }
  }

  login(username: string, password: string): void {
    this.password = password;
    this.username = username;
    this.websocketService.sendObject({"cmd": "authenticate", "user": username, "password": password});
  }

  isAuthenticated(): boolean {
    return this.authenticated;
  }

  hasPermission(permission: string): boolean {
    if (!this.isAuthenticated()) {
      return false;
    }
    return this.permissions.includes(permission);
  }
}
