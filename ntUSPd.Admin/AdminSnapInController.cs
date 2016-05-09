namespace ntUSPd.Admin
{
	using System;
	using System.Collections.Generic;
	using System.Linq;
	using System.Text;
	using System.Threading.Tasks;
	using Microsoft.ManagementConsole;

	[SnapInSettings(
		"{2FA4F7B3-C42C-44C9-A08E-A147D059B612}",
		DisplayName = "Network UPS Tools Services",
		Description = "Manages ntUPSd service")]
	public class AdminSnapInController : SnapIn
	{
		public AdminSnapInController()
		{
			this.RootNode = new ScopeNode();
			this.RootNode.DisplayName = "Network UPS Services";
		}
	}
}
