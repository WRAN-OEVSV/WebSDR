import {Component, Inject} from '@angular/core';
import {MAT_DIALOG_DATA, MatDialogRef} from "@angular/material/dialog";

@Component({
  selector: 'app-login-form',
  templateUrl: './login-form.component.html',
  styleUrls: ['./login-form.component.css']
})
export class LoginFormComponent {
  username: string = "";
  password: string = "";


  constructor(public dialogRef: MatDialogRef<LoginFormComponent>,
              @Inject(MAT_DIALOG_DATA) public data: any) {
  }
  authenticate() {
    this.dialogRef.close({reason: "login", username: this.username, password: this.password});
  }

  setPassword($event: KeyboardEvent) {
    const tg = $event.target as HTMLInputElement;
    this.password = tg.value;
  }

  setUsername($event: KeyboardEvent) {
    const tg = $event.target as HTMLInputElement;
    this.username = tg.value;
  }

  abort() {
    this.dialogRef.close({reason: "abort"});
  }
}
