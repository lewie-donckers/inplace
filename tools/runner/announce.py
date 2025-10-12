try:
    import colorama

    use_colorama = True
except:
    use_colorama = False


class Announcer:
    _style_base = colorama.Style.BRIGHT if use_colorama else ""
    _style_announce = colorama.Fore.YELLOW if use_colorama else ""
    _style_alt = colorama.Fore.CYAN if use_colorama else ""
    _style_error = colorama.Fore.RED if use_colorama else ""
    _style_reset = colorama.Style.RESET_ALL if use_colorama else ""

    def __call__(self, message):
        self.announce(message)

    def announce(self, message):
        self._announce(message, self._style_announce)

    def alt(self, message):
        self._announce(message, self._style_alt)

    def error(self, message):
        self._announce(message, self._style_error)

    def _announce(self, message, style):
        print(self._style_base + style + message + self._style_reset)


announce = Announcer()
